#ifndef PUMP_H
#define PUMP_H
#include "profileManager.h"
#include "log.h"
#include "home.h"
#include "bolus.h"
#include <string>
#include <QObject>
#include <QDebug>

class Pump : public QObject{
    Q_OBJECT

    private:
    ProfileManager* profileManager;
    Profile* currentProfile;
    Home* home;
    Log* log;
    Bolus* bolus;


    // following variables are added to manage insulin delivery and bolus
    bool insulinDeliveryActive;
    float currentGlucoseLevel;

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
    void startInsulinDelivery();
    void stopInsulinDelivery();
    void resumeInsulinDelivery();

    // Bolus insulin management - Requirement 4
    void deliverBolus(float glucoseLevel, float carbIntake);
    void deliverExtendedBolus(float glucoseLevel, int duration);
    void deliverQuickBolus(float glucoseLevel, int duration);

    // bolus control methods - Requirement 4
    void pauseBolus();
    void resumeBolus();
    void cancelBolus();

    void simulate();

    // Profile management - Requirement 3
    void createUserProfile(const std::string& mode, float basalRate, float correctionFactor,
                       float carbRatio, float targetGlucose, std::string& errMsg);
    void switchProfile(const std::string& mode);

    ProfileManager* getProfileManager() { return profileManager; }

    void setCurrentProfile(Profile* p){ this->currentProfile = p; }
    Profile* getCurrentProfile() { return currentProfile; }
    Bolus* getBolus() {return bolus;}
    Bolus* setBolus(Bolus* b){  this->bolus = b;  return this->bolus; }
    Home* getHome() {return home;}
    int getInsulinDoseRemaining();
    void adjustGlucoseLevel();

    void setCurrentGlucoseLevel(float level);
    float getCurrentGlucoseLevel() const;

    public slots:
    // Alert handling slots
    void handleLowBatteryWarning(float level);
    void handleCriticalBatteryWarning(float level);
    void handleLowInsulinWarning(int remaining);
    void emergencyShutdown();
};
#endif // PUMP_H
