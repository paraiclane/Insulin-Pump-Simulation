#include "home.h"
#include "profile.h"
#include <QDebug>

Home::Home(QObject *parent)
    : QObject(parent),
      powerOff(false),
      charging(false),
      blocked(false),
      batteryLevel(100.0f),
      insulinDoseRemaining(300),
      iob(0.0f),
      updateTimer(nullptr),
      glucoseLevel()
{
    initializeTimer();
}

Home::~Home()
{
    if (updateTimer) {
        updateTimer->stop();
        delete updateTimer;
    }
}

void Home::initializeTimer()
{
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Home::onTimerTick);
    updateTimer->start(60000); // update every minute
    qDebug() << "Timer initialized for background updates";
}

void Home::onTimerTick()
{
    // Update time
    time =  QDateTime::currentDateTime();

    // Handle battery drain and charging
    if (charging && batteryLevel < 100.0f) {
        setBatteryLevel(batteryLevel + CHARGE_RATE);
    } else if (!charging && batteryLevel > 0.0f) {
        // Background battery drain
        setBatteryLevel(batteryLevel - PASSIVE_DRAIN_RATE);
    }

    // Check for alerts
    checkBatteryAlert();
    checkInsulinRemainingAlert();

    // Auto-shutdown if battery is critically low
    if (isPowerCritical() && !charging) {
        qDebug() << "Critical battery level reached. Auto shutdown initiated.";
        emit powerShutDown();
    }

    // Decay IOB over time
    if (iob > 0.0f) {
        // Reduce IOB by some amount each minute
        updateIOB(iob - 0.01f);
    }
}

float Home::getBatteryLevel() { return batteryLevel; }
bool Home::getCharging() { return charging; }
int Home::getInsulinDoseRemaining() { return insulinDoseRemaining; }

bool Home::isPowerCritical() const
{
    return batteryLevel <= CRITICAL_BATTERY_THRESHOLD;
}

void Home::selectProfile(Profile *profile)
{
   currentProfile = profile;
   emit profileChanged(profile);
}

void Home::usePower()
{
    if(!charging && batteryLevel > 0) {
        setBatteryLevel(batteryLevel - ACTIVE_DRAIN_RATE);
    }
    if(charging && batteryLevel < 100){
        setBatteryLevel(batteryLevel + CHARGE_RATE);
    }
}

void Home::chargePower()
{
   charging = true;
   qDebug() << "Charging started";
}

void Home::stopCharging()
{
    charging = false;
    qDebug() << "Charging stopped";
}

void Home::checkBatteryAlert()
{
    if (batteryLevel <= CRITICAL_BATTERY_THRESHOLD) {
        emit criticalBatteryWarning(batteryLevel);
        qDebug() << "CRITICAL BATTERY WARNING: " << batteryLevel << "% remaining";
    }
    else if (batteryLevel < LOW_BATTERY_THRESHOLD) {
        emit lowBatteryWarning(batteryLevel);
        qDebug() << "Low battery warning: " << batteryLevel << "% remaining";
    }
}

void Home::checkInsulinRemainingAlert()
{
    if(insulinDoseRemaining < LOW_INSULIN_THRESHOLD) {
        emit insulinLowWarning(insulinDoseRemaining);
        qDebug() << "Low insulin warning: " << insulinDoseRemaining << " units remaining";
    }
}

void Home::checkOcclusion()
{
    if(blocked == true){
        emit occlusionDetected();
        qDebug() << "Occlusion detected!";
    }
}

void Home::setBatteryLevel(float battery)
{
    // To ensure battery level stays within valid range
    if (battery > 100.0f) {
        batteryLevel = 100.0f;
    } else if (battery < 0.0f) {
        batteryLevel = 0.0f;
    } else {
        batteryLevel = battery;
    }
}


void Home::setInsulinRemaining(int amount)
{
    // Ensure insulin amount is never negative
    insulinDoseRemaining = (amount < 0) ? 0 : amount;
}

void Home::updateIOB(float amount)
{
    // Ensure IOB is never negative
    iob = (amount < 0.0f) ? 0.0f : amount;
}

void Home::setGlucoseLevel(float g){
    glucoseLevel = g;
}
