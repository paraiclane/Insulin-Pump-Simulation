#ifndef HOME_H
#define HOME_H

#include "profile.h"

#include <QObject>
#include <QDateTime>
#include <QTimer>

class Home : public QObject
{
    Q_OBJECT

public:
    explicit Home(QObject *parent = nullptr);
    ~Home();

    QDateTime time;

    // Battery management
    float getBatteryLevel();
    bool getCharging();
    bool isPowerCritical() const; // check if battery is critically low

    //insulin management
    int getInsulinDoseRemaining();
    float getIOB() const { return iob; } // getter for IOB
    void updateIOB(float amount); // update IOB
    void adjustGlucoseLevel(float targetGlucose);
    float getGlucoseLevel(){return glucoseLevel;}
    void setGlucoseLevel(float g);

    // Profile management
    void selectProfile(Profile *profile);

    // Power management
    void usePower();
    void chargePower();
    void stopCharging(); // Stop charging

    // Check Alerts
    void checkBatteryAlert();
    void checkInsulinRemainingAlert();
    void checkOcclusion();


signals:

    void profileChanged(Profile *newProfile);
    void lowBatteryWarning(float level);
    void criticalBatteryWarning(float level); // for ritical battery level
    void insulinLowWarning(int remaining);
    void occlusionDetected();
    void powerShutDown(); // for automatic shutdown (I think we need it but do we just shotdown the program?)

public slots:
    void onTimerTick(); // handling timer updates

private:
    void setBatteryLevel(float battery);
    void setInsulinRemaining(int amount); // setter for insulin
    void initializeTimer(); // set up the timer

    Profile *currentProfile = nullptr;
    bool powerOff;
    bool charging;
    bool blocked;
    float batteryLevel;
    int insulinDoseRemaining;
    float iob;
    float glucoseLevel;
    QTimer *updateTimer;

    // Constants
    const float CRITICAL_BATTERY_THRESHOLD = 5.0f;
    const float LOW_BATTERY_THRESHOLD = 20.0f;
    const float LOW_INSULIN_THRESHOLD = 50;
    const float PASSIVE_DRAIN_RATE = 0.01f; // Battery drain per timer tick
    const float ACTIVE_DRAIN_RATE = 0.1f;   // Battery drain per active operation
    const float CHARGE_RATE = 0.2f;         // Battery charge rate when charging
};

#endif
