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
        connect(home, &Home::batteryLevelChanged, this, &Pump::batteryLevelChanged);
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
    std::string errorMsg;  // Create a local error message variable
    stopInsulinDelivery(errorMsg);
    updateLog("[Alert] CGM Alert triggered!");
}

// Occlusion alert - Requirement 7
void Pump::occlusionAlert() {
    if (home) {
        home->checkOcclusion();
    }
    std::string errorMsg;  // Create a local error message variable
    stopInsulinDelivery(errorMsg);  // pass to the function
    updateLog("[Alert] Occlusion Alert triggered! Insulin delivery stopped.");
}

// Start insulin delivery - Requirement 5
bool Pump::startInsulinDelivery(std::string& errorMsg) {
    InsulinDeliveryState oldState = deliveryState;
    // Safety check
    if (!currentProfile) {
        errorMsg = "No active profile selected.";
        updateLog("[Error] Cannot start insulin delivery: " + errorMsg);
        return false;
    }

    // Check glucose level safety
    if (currentGlucoseLevel < 70.0f) {  // Assuming 70 mg/dL is the hypoglycemia threshold
        errorMsg = "Current glucose level is too low.";
        updateLog("[Alert] Cannot start insulin delivery: " + errorMsg);
        deliveryState = InsulinDeliveryState::SUSPENDED;
        logStateChange(oldState, deliveryState);
        emit deliveryStateChanged(deliveryState);
        return false;
    }

    // Start delivery
    insulinDeliveryActive = true;
    deliveryState = InsulinDeliveryState::ACTIVE;
    updateLog("[Pump] Insulin delivery started.");
    logStateChange(oldState, deliveryState);
    emit deliveryStateChanged(deliveryState);
    return true;
}

// Stop insulin delivery - Requirement 5
bool Pump::stopInsulinDelivery(std::string& errorMsg) {
    InsulinDeliveryState oldState = deliveryState;

    if (deliveryState == InsulinDeliveryState::INACTIVE) {
        errorMsg = "Insulin delivery is already inactive.";
        return true; // Not an error, its already inactive
    }

    insulinDeliveryActive = false;
    deliveryState = InsulinDeliveryState::PAUSED;
    updateLog("[PUMP] Insulin delivery stopped manually.");
    logStateChange(oldState, deliveryState);
    emit deliveryStateChanged(deliveryState);
    return true;
}

// Resume insulin delivery - Requirement 5
bool Pump::resumeInsulinDelivery(std::string& errorMsg) {
    InsulinDeliveryState oldState = deliveryState;
    // Safety check
    if (!currentProfile) {
        errorMsg = "No active profile selected.";
        updateLog("[Error] Cannot resume insulin delivery: " + errorMsg);
        return false;
    }

    // Check if the state is paused or suspended
    if (deliveryState != InsulinDeliveryState::PAUSED && deliveryState != InsulinDeliveryState::SUSPENDED) {
        errorMsg = "Insulin delivery is not paused or suspended.";
        updateLog("[Info] " + errorMsg);
        return false;
    }

    // If suspended due to low glucose, check if glucose level has recovered
    if (deliveryState == InsulinDeliveryState::SUSPENDED) {
        if (currentGlucoseLevel < 80.0f) {  // 80 mg/dL as the safe number
            errorMsg = "Glucose level still too low.";
            updateLog("[Alert] Cannot resume insulin delivery: " + errorMsg);
            return false;
        }
    }

    // Resume delivery
    insulinDeliveryActive = true;
    deliveryState = InsulinDeliveryState::ACTIVE;
    updateLog("[PUMP] Insulin delivery resumed.");
    logStateChange(oldState, deliveryState);
    emit deliveryStateChanged(deliveryState);
    return true;
}

// Deliver bolus insulin - Requirement 4
bool Pump::deliverBolus(float glucoseLevel, float carbIntake, std::string& errorMsg) {
    if (!insulinDeliveryActive) {
        errorMsg = "Insulin delivery not active.";
        updateLog("[Bolus] Cannot deliver bolus: " + errorMsg);
        emit bolusDeliveryFailed(errorMsg);
        return false;
    }
    if (!currentProfile) {
        errorMsg = "No active profile selected.";
        updateLog("[Bolus] Cannot deliver bolus: " + errorMsg);
        emit bolusDeliveryFailed(errorMsg);
        return false;
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
    emit bolusDeliveryComplete();
    return true;
}

// Exteneded bolus - Requirement 4
bool Pump::deliverExtendedBolus(float glucoseLevel, int duration, std::string& errorMsg) {
    if (!insulinDeliveryActive) {
        errorMsg = "Insulin delivery not active.";
        updateLog("[Bolus] Cannot deliver bolus: " + errorMsg);
        emit bolusDeliveryFailed(errorMsg);
        return false;
    }

    if (!currentProfile) {
        errorMsg = "No active profile selected.";
        updateLog("[Bolus] Cannot deliver extended bolus: " + errorMsg);
        emit bolusDeliveryFailed(errorMsg);
        return false;
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
    emit bolusDeliveryComplete();
    return true;
}

// Quick bolus - Requirement 4
bool Pump::deliverQuickBolus(float glucoseLevel, int duration, std::string& errorMsg) {
    if (!insulinDeliveryActive) {
        errorMsg = "Insulin delivery not active.";
        updateLog("[Bolus] Cannot deliver quick bolus: " + errorMsg);
        return false;
    }

    if (!currentProfile) {
        errorMsg = "No active profile selected.";
        updateLog("[Bolus] Cannot deliver quick bolus: " + errorMsg);
        return false;
    }

    // Create a temporary bolus with the current profile
    std::string bolusID = "[Bolus] QuickBolus-" + std::to_string(time(nullptr));
    Bolus tempBolus(bolusID, glucoseLevel, 0, currentProfile);

    // Calculate the appropriate dose
    tempBolus.calculateFinalBolus();

    tempBolus.quickBolus();

    updateLog("[Bolus] Quick bolus delivered");
    emit bolusDeliveryComplete();
    return true;
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
bool Pump::createUserProfile(const std::string& mode, float basalRate, float correctionFactor,
                             float carbRatio, float targetGlucose, std::string& errorMsg) {
    if (profileManager) {
        bool success = profileManager->createProfile(mode, basalRate, correctionFactor, carbRatio,
                                                     targetGlucose, errorMsg);
        if (success) {
            updateLog("[Profile] Created new profile: " + mode);
            return true;
        } else {
            updateLog("[Profile] Failed to create profile: " + errorMsg);
            return false;
        }
    }
    errorMsg = "Profile manager is not initialized";
    return false;
}

// switch user profile - Requirement 3
bool Pump::switchProfile(const std::string& mode, std::string& errorMsg) {
    if (profileManager) {
        Profile* selectedProfile = profileManager->readProfile(mode);
        if (selectedProfile) {
            currentProfile = selectedProfile;
            if (home) {
                home->selectProfile(selectedProfile);
            }
            updateLog("[Profile] Switched to profile: " + mode);
            return true;
        } else {
            errorMsg = "Failed to find profile: " + mode;
            updateLog("[Profile] Failed to switch to profile: " + errorMsg);
            return false;
        }
    }
    errorMsg = "Profile manager is not initialized";
    return false;
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
    std::string errorMsg;  // Create a local error message variable
    stopInsulinDelivery(errorMsg);  // pass to the function
    if (bolus && !bolus->isCanceled()) {
        cancelBolus();
    }
    powerOff();;
}

void Pump::checkGlucoseSafety() {
    if (deliveryState == InsulinDeliveryState::ACTIVE && currentGlucoseLevel < 70.0f) {
        InsulinDeliveryState oldState = deliveryState;

        // Automatically suspend delivery
        insulinDeliveryActive = false;
        deliveryState = InsulinDeliveryState::SUSPENDED;
        updateLog("[Alert] Insulin delivery automatically suspended due to low glucose level.");
        logStateChange(oldState, deliveryState);

        emit deliveryStateChanged(deliveryState);

        // Trigger CGM alert
        triggerCGMAlert();
    }
}

// Log state changes
void Pump::logStateChange(InsulinDeliveryState oldState, InsulinDeliveryState newState) {
    if (oldState == newState) {
        return;
    }

    std::string oldStateStr, newStateStr;

    // Convert enum to string
    switch (oldState) {
    case InsulinDeliveryState::INACTIVE: oldStateStr = "INACTIVE"; break;
    case InsulinDeliveryState::ACTIVE: oldStateStr = "ACTIVE"; break;
    case InsulinDeliveryState::PAUSED: oldStateStr = "PAUSED"; break;
    case InsulinDeliveryState::SUSPENDED: oldStateStr = "SUSPENDED"; break;
    }

    switch (newState) {
    case InsulinDeliveryState::INACTIVE: newStateStr = "INACTIVE"; break;
    case InsulinDeliveryState::ACTIVE: newStateStr = "ACTIVE"; break;
    case InsulinDeliveryState::PAUSED: newStateStr = "PAUSED"; break;
    case InsulinDeliveryState::SUSPENDED: newStateStr = "SUSPENDED"; break;
    }

    updateLog("[STATE] Insulin delivery state changed: " + oldStateStr + " -> " + newStateStr);
}

// Set the current glucose level
void Pump::setCurrentGlucoseLevel(float level) {
    float oldLevel = currentGlucoseLevel;
    currentGlucoseLevel = level;

    updateLog("[CGM] Glucose level updated: " + std::to_string(level) + " mg/dL");

    // If glucose level changes significantly, check safety
    if (std::abs(oldLevel - level) > 15.0f) {
        checkGlucoseSafety();
    }
}

// Get insulin delivery state
InsulinDeliveryState Pump::getDeliveryState() const {
    return deliveryState;
}

// Get the current glucose level
float Pump::getCurrentGlucoseLevel() const {
    return currentGlucoseLevel;
}

int Pump::getInsulinDoseRemaining()
{
    return home ? home->getInsulinDoseRemaining() : 0;
}
