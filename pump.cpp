#include "pump.h"
#include <iostream>
#include <ctime>

// Constructor
Pump::Pump(ProfileManager* pm, Home* h, Log* l)
    : profileManager(pm), home(h), log(l), bolus(nullptr), insulinDeliveryActive(false), currentProfile(nullptr), currentGlucoseLevel(120.0f) {
    // Connect to Home signals for alerts
    if (home) {
        connect(home, &Home::lowBatteryWarning, this, &Pump::handleLowBatteryWarning);
        connect(home, &Home::criticalBatteryWarning, this, &Pump::handleCriticalBatteryWarning);
        connect(home, &Home::insulinLowWarning, this, &Pump::handleLowInsulinWarning);
        connect(home, &Home::occlusionDetected, this, &Pump::occlusionAlert);
        connect(home, &Home::powerShutDown, this, &Pump::emergencyShutdown);
    }
}

// Destructor
Pump::~Pump() {
    // Destructor does not need to delete pointers because they are passed in externally
}

// Update to log
void Pump::updateLog(const std::string& s) {
    if (log) {
        log->appendText(s);
    }
}

// Power on - Requirement 2
void Pump::powerOn() {
    if (home) {
        home->usePower(); // from Home class
    }
    updateLog("Pump powered on.");
}

// Power off
void Pump::powerOff() {
    stopInsulinDelivery();
    updateLog("Pump powered off.");
}

// battery management

void Pump::startCharging()
{
    if (home) {
        home->chargePower();
        updateLog("[System] Charging started");
    }
}

void Pump::stopCharging()
{
    if (home) {
        home->stopCharging();
        updateLog("[System] Charging stopped");
    }
}

float Pump::getBatteryLevel()
{
    return home ? home->getBatteryLevel() : 0.0f;
}

bool Pump::isCharging()
{
    return home ? home->getCharging() : false;
}

// CGM alert - Related to Requirements 5 and 7
void Pump::triggerCGMAlert() {
    updateLog("[Alert] CGM Alert triggered!");
    // Automatically stop insulin delivery when blood glucose is below the threshold
    stopInsulinDelivery();
}

// Occlusion alert - Requirement 7
void Pump::occlusionAlert() {
    if (home) {
        home->checkOcclusion();
    }
    stopInsulinDelivery();
    updateLog("[Alert] Occlusion Alert triggered! Insulin delivery stopped.");
}

// Start insulin delivery - Requirement 5
void Pump::startInsulinDelivery() {
    insulinDeliveryActive = true;
    updateLog("[Pump] Insulin delivery started.");
}

// Stop insulin delivery - Requirement 5
void Pump::stopInsulinDelivery() {
    insulinDeliveryActive = false;
    updateLog("[Pump] Insulin delivery stopped.");
}

// Resume insulin delivery - Requirement 5
void Pump::resumeInsulinDelivery() {
    insulinDeliveryActive = true;
    updateLog("[Pump] Insulin delivery resumed.");
}

// Deliver bolus insulin - Requirement 4
void Pump::deliverBolus(float glucoseLevel, float carbIntake) {
    if (!insulinDeliveryActive) {
        updateLog("[Bolus] Cannot deliver bolus: insulin delivery not active.");
        return;
    }
    if (!currentProfile) {
        updateLog("[Bolus] Cannot deliver bolus: no active profile selected.");
        return;
    }

    // Create a unique ID for this bolus
    std::string bolusID = "Bolus-" + std::to_string(time(nullptr));

    // Create a new bolus with the current profile
    Bolus tempBolus(bolusID, glucoseLevel, carbIntake, currentProfile);

    // Calculate the appropriate dose
    tempBolus.calculateFinalBolus();
    float calculatedDose = tempBolus.getAppropriateDose();
    updateLog("[Bolus] Calculated bolus dose: " + std::to_string(calculatedDose) + " units");

    updateLog("[Bolus] Bolus delivery confirmed: " + std::to_string(calculatedDose) + " units");
}

// Exteneded bolus - Requirement 4
void Pump::deliverExtendedBolus(float glucoseLevel, int duration) {
    if (!insulinDeliveryActive || !currentProfile) {
        updateLog("[Bolus] Cannot deliver extended bolus: insulin delivery not active or no profile selected.");
        return;
    }

    // Create a temporary bolus with the current profile
    std::string bolusID = "ExtBolus-" + std::to_string(time(nullptr));
    Bolus tempBolus(bolusID, glucoseLevel, 0, currentProfile);

    // Calculate the appropriate dose
    tempBolus.calculateFinalBolus();

    // Convert duration from minutes to hours
    int hours = duration / 60;
    if (hours < 1) hours = 1;

    tempBolus.extendedBolus(hours);

    updateLog("[Bolus] Extended bolus started for " + std::to_string(duration) + " minutes");
}

// Quick bolus - Requirement 4
void Pump::deliverQuickBolus(float glucoseLevel, int duration) {
    if (!insulinDeliveryActive || !currentProfile) {
        updateLog("[Bolus] Cannot deliver quick bolus: insulin delivery not active or no profile selected.");
        return;
    }

    // Create a temporary bolus with the current profile
    std::string bolusID = "[Bolus] QuickBolus-" + std::to_string(time(nullptr));
    Bolus tempBolus(bolusID, glucoseLevel, 0, currentProfile);

    // Calculate the appropriate dose
    tempBolus.calculateFinalBolus();

    tempBolus.quickBolus();

    updateLog("[Bolus] Quick bolus delivered");
}

// Pause bolus delivery - Requirement 4
void Pump::pauseBolus() {
    if (!bolus) {
        updateLog("[Bolus] No active bolus to pause.");
        return;
    }

    if (bolus->isActive()) {
        bolus->pauseDelivery();
        updateLog("[Bolus] Bolus delivery paused.");
    } else if (bolus->isPaused()) {
        updateLog("[Bolus] Bolus is already paused.");
    } else if (bolus->isCanceled()) {
        updateLog("[Bolus] Cannot pause: Bolus has been canceled.");
    }
}

// Resume bolus delivery - Requirement 4
void Pump::resumeBolus() {
    if (!bolus) {
        updateLog("[Bolus] No bolus to resume.");
        return;
    }

    if (!insulinDeliveryActive) {
        updateLog("[Bolus] Cannot resume bolus: insulin delivery not active.");
        return;
    }

    if (bolus->isPaused()) {
        bolus->resumeDelivery();
        updateLog("[Bolus] Bolus delivery resumed.");
    } else if (bolus->isActive()) {
        updateLog("[Bolus] Bolus is already active.");
    } else if (bolus->isCanceled()) {
        updateLog("[Bolus] Cannot resume: Bolus has been canceled.");
    }
}

// Cancel bolus delivery - Requirement 4
void Pump::cancelBolus() {
    if (!bolus) {
        updateLog("[Bolus] No active bolus to cancel.");
        return;
    }

    if (!bolus->isCanceled()) {
        bolus->cancelDelivery();
        updateLog("[Bolus] Bolus delivery canceled.");
    } else {
        updateLog("[Bolus] Bolus is already canceled.");
    }
}

// cancel bolus - Requirement 4
/*void Pump::cancelBolus() {
    if (bolus) {
        bolus->cancelDelivery();
        updateLog("[Bolus] Bolus delivery canceled");
    }
}*/

// create user profile - Requirement 3
void Pump::createUserProfile(const std::string& mode, float basalRate, float correctionFactor,
                         float carbRatio, float targetGlucose, std::string& errMsg) {
    if (profileManager) {
        profileManager->createProfile(mode, basalRate, correctionFactor, carbRatio, targetGlucose,errMsg);
        updateLog("[Profile] Created new profile: " + mode);
    }
}

// switch user profile - Requirement 3
void Pump::switchProfile(const std::string& mode) {
    if (profileManager) {
        Profile* selectedProfile = profileManager->readProfile(mode);
        if (selectedProfile) {
            currentProfile = selectedProfile;
            if (home) {
                home->selectProfile(selectedProfile);
            }
            updateLog("[Profile] Switched to profile: " + mode);
        } else {
            updateLog("[Profile] Failed to switch to profile: " + mode);
        }
    }
}

// Battery alert
void Pump::handleLowBatteryWarning(float level)
{
    updateLog(QString("[Alert] Low battery warning: %1% remaining").arg(level).toStdString());
}

void Pump::handleCriticalBatteryWarning(float level)
{
    updateLog(QString("[Alert] CRITICAL BATTERY WARNING: %1% remaining").arg(level).toStdString());
    updateLog("[System] Please connect charger immediately to prevent shutdown");
}

void Pump::handleLowInsulinWarning(int remaining)
{
    updateLog(QString("[Alert] Low insulin warning: %1 units remaining").arg(remaining).toStdString());
}

void Pump::emergencyShutdown()
{
    updateLog("[System] Emergency shutdown initiated due to critical battery level");
    stopInsulinDelivery();
    if (bolus && !bolus->isCanceled()) {
        cancelBolus();
    }
    powerOff();;
}

void Pump::adjustGlucoseLevel(){
    if(currentProfile->getTargetGlucoseLevels() > home->getGlucoseLevel()){
        home->setGlucoseLevel(home->getGlucoseLevel() + 0.1);
    }
    else if(currentProfile->getTargetGlucoseLevels() < home->getGlucoseLevel()){
        home->setGlucoseLevel(home->getGlucoseLevel() - 0.1);
    }
}

float Pump::getCurrentGlucoseLevel() const {
    return currentGlucoseLevel;
}

int Pump::getInsulinDoseRemaining()
{
    return home ? home->getInsulinDoseRemaining() : 0;
}
