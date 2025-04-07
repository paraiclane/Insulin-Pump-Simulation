#ifndef PUMP_H
#define PUMP_H
#include "profileManager.h"
#include "log.h"
#include "home.h"
#include "bolus.h"
#include <string>
#include <QObject>
#include <QDebug>


enum class InsulinDeliveryState {
    INACTIVE,  // Not started
    ACTIVE,    // Delivering normally
    PAUSED,    // Manually paused
    SUSPENDED  // Automatically paused for safety reasons (e.g., hypoglycemia)
};

class Pump : public QObject{
    Q_OBJECT

    private:
    ProfileManager* profileManager;
    Profile* currentProfile;
    Home* home;
    Log* log;
    Bolus* bolus;
    InsulinDeliveryState deliveryState;
    float currentGlucoseLevel;  // Current blood glucose level

    void checkGlucoseSafety();

    // Log state changes
    void logStateChange(InsulinDeliveryState oldState, InsulinDeliveryState newState);

    // following variables are added to manage insulin delivery and bolus
    bool insulinDeliveryActive;

    public:
    // Constructor
    Pump(ProfileManager* pm, Home* h, Log* l);
    ~Pump();

    // Log and power management
    void updateLog(const std::string& s);
    void powerOn();
    void powerOff();

    // Battery management
    void startCharging();
    void stopCharging();
    float getBatteryLevel();
    bool isCharging();

    // Alert system
    void triggerCGMAlert();
    void occlusionAlert();

    // Insulin delivery control - Requirement 5
    bool startInsulinDelivery(std::string& errorMsg);
    bool stopInsulinDelivery(std::string& errorMsg);
    bool resumeInsulinDelivery(std::string& errorMsg);


    void setCurrentGlucoseLevel(float level);
    float getCurrentGlucoseLevel() const;
    InsulinDeliveryState getDeliveryState() const;


    // Bolus insulin management - Requirement 4
    void deliverBolus(float glucoseLevel, float carbIntake);
    void deliverExtendedBolus(float glucoseLevel, int duration);
    void deliverQuickBolus(float glucoseLevel, int duration);
    int getInsulinDoseRemaining();

    // bolus control methods - Requirement 4
    void pauseBolus();
    void resumeBolus();
    void cancelBolus();

    void simulate();

    // Profile management - Requirement 3
    void createUserProfile(const std::string& mode, float basalRate, float correctionFactor,
                       float carbRatio, float targetGlucose, std::string& errMsg);
    void switchProfile(const std::string& mode, std::string& errorMsg);

    ProfileManager* getProfileManager() { return profileManager; }

    void setCurrentProfile(Profile* p){ this->currentProfile = p; }
    Profile* getCurrentProfile() { return currentProfile; }
    Bolus* getBolus() {return bolus;}
    Bolus* setBolus(Bolus* b){  this->bolus = b;  return this->bolus; }
    Home* getHome() {return home;}
    void adjustGlucoseLevel();

    void setCurrentGlucoseLevel(float level);
    float getCurrentGlucoseLevel() const;

    public slots:
    // Alert handling slots
    void handleLowBatteryWarning(float level);
    void handleCriticalBatteryWarning(float level);
    void handleLowInsulinWarning(int remaining);
    void emergencyShutdown();

    signals:
    void deliveryStateChanged(InsulinDeliveryState state);
    void bolusDeliveryFailed(const std::string& errorMsg);
    void bolusDeliveryComplete();
    void batteryLevelChanged(float level);
};
#endif // PUMP_H
