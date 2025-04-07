#include "bolus.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

// Constructor
Bolus::Bolus(const string& bolusID, float glucoseLevel, float carbIntake, Profile* profile)
  : bolusID(bolusID), glucoseLevel(glucoseLevel), carbIntake(carbIntake),
    appropriateDose(0.0f), iob(0.0f), patientProfile(profile)
{
}

// Calculates final bolus dose based on user input and profile settings
void Bolus::calculateFinalBolus()
{
  if (!patientProfile) {
      cerr << "[Error] No profile available. Cannot calculate bolus with missing ratios.\n";
      appropriateDose = 0.0f;
      return;
  }

  float icr = patientProfile->getCarbohydratesRatio();
  float cf = (overrideCorrectionFactor > 0.0f) ? overrideCorrectionFactor : patientProfile->getCorrectionFactor();
  float targetBG = patientProfile->getTargetGlucoseLevels();

  if (icr <= 0.0f || cf <= 0.0f || targetBG <= 0.0f) {
      cerr << "[Error] Invalid profile settings: ICR, Correction Factor, and Target BG must all be greater than 0.\n";
      appropriateDose = 0.0f;
      return;
  }

  float foodBolus = carbIntake / icr;
  float correctionBolus = max((glucoseLevel - targetBG) / cf, 0.0f);

  float totalBolus = foodBolus + correctionBolus;
  appropriateDose = max(totalBolus - iob, 0.0f);

  cout << "[Calculation Summary]\n";
  cout << "  Food Bolus:       " << foodBolus << " units\n";
  cout << "  Correction Bolus: " << correctionBolus << " units\n";
  cout << "  IOB:              " << iob << " units\n";
  cout << "  Final Bolus:      " << appropriateDose << " units\n";
}

// Recalculate after input change
void Bolus::adjustDose()
{
  calculateFinalBolus();
}

// Quick bolus: immediate 60%
void Bolus::quickBolus()
{
  float dose = 0.6f * appropriateDose;
  cout << "Quick Bolus: " << dose << " units delivered immediately.\n";
}

// Extended bolus: 40% over time (no simulation, just math)
void Bolus::extendedBolus(int durationHours) {
    float dose = 0.4f * appropriateDose;

    if (durationHours <= 0) {
        cout << "[Warning] Invalid extended bolus time. Defaulting to 1 hour.\n";
        durationHours = 1;
    }

    float hourlyRate = dose / durationHours;
    cout << "Extended Bolus: " << dose << " units scheduled over " << durationHours
         << " hours (" << hourlyRate << " units/hour).\n";
}

// Pause bolus delivery
void Bolus::pauseDelivery() {
    if (deliveryState == ACTIVE) {
        deliveryState = PAUSED;
        cout << "Bolus " << bolusID << " delivery paused." << endl;
    } else if (deliveryState == CANCELED) {
        cout << "Cannot pause: Bolus " << bolusID << " has already been canceled." << endl;
    } else {
        cout << "Bolus " << bolusID << " is already paused." << endl;
    }
}

// Resume bolus delivery
void Bolus::resumeDelivery() {
    if (deliveryState == PAUSED) {
        deliveryState = ACTIVE;
        cout << "Bolus " << bolusID << " delivery resumed." << endl;
    } else if (deliveryState == CANCELED) {
        cout << "Cannot resume: Bolus " << bolusID << " has been canceled." << endl;
    } else {
        cout << "Bolus " << bolusID << " is already active." << endl;
    }
}

// Cancel bolus delivery
void Bolus::cancelDelivery() {
    if (deliveryState != CANCELED) {
        deliveryState = CANCELED;
        cout << "Bolus " << bolusID << " delivery canceled." << endl;
    } else {
        cout << "Bolus " << bolusID << " is already canceled." << endl;
    }
}

// check states
bool Bolus::isActive() const {
    return deliveryState == ACTIVE;
}

bool Bolus::isPaused() const {
    return deliveryState == PAUSED;
}

bool Bolus::isCanceled() const {
    return deliveryState == CANCELED;
}

// Getters
string Bolus::getBolusID() const { return bolusID; }
float Bolus::getGlucoseLevel() const { return glucoseLevel; }
float Bolus::getCarbIntake() const { return carbIntake; }
float Bolus::getAppropriateDose() const { return appropriateDose; }

// Setters
void Bolus::setGlucoseLevel(float glucose)
{
  glucoseLevel = glucose;
  calculateFinalBolus();
}

void Bolus::setCarbIntake(float carbs)
{
  carbIntake = carbs;
  calculateFinalBolus();
}

void Bolus::setAppropriateDose(float dose)
{
  appropriateDose = dose;
}

void Bolus::setCorrectionFactorOverride(float cf) {
   overrideCorrectionFactor = cf;
}

void Bolus::setIOB(float insulinOnBoard)
{
   iob = insulinOnBoard;
}

void Bolus::setExtendedDuration(int hours) {
    extendedDurationHours = hours;
}

void Bolus::setActive(){
    if(deliveryState == CANCELED){
        return;
    }
    deliveryState = ACTIVE;
}

void Bolus::setPaused(){
   if(deliveryState == CANCELED){
       return;
   }
    deliveryState = PAUSED;
}

void Bolus::setCanceled(){
    deliveryState = CANCELED;
}
