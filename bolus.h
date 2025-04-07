#ifndef BOLUS_H
#define BOLUS_H

#include <string>
#include "profile.h"

class Bolus {
public:
   Bolus(const std::string& bolusID, float glucoseLevel, float carbIntake, Profile* profile);

   void calculateFinalBolus();
   void adjustDose(); // Recalculate if values change
   void quickBolus();
   void extendedBolus(int durationHours);

   // bolus control
   void pauseDelivery();
   void resumeDelivery();
   void cancelDelivery();
   bool isActive() const;
   bool isPaused() const;
   bool isCanceled() const;

   // Getters
   std::string getBolusID() const;
   float getGlucoseLevel() const;
   float getCarbIntake() const;
   float getAppropriateDose() const;
   float getDeliveryState() { return deliveryState; }
   // Setters
   void setGlucoseLevel(float glucose);
   void setCarbIntake(float carbs);
   void setAppropriateDose(float dose);
   void setCorrectionFactorOverride(float cf);
   void setIOB(float insulinOnBoard);
   void setExtendedDuration(int hours);
   void setActive();
   void setPaused();
   void setCanceled();



private:
   std::string bolusID;
   float glucoseLevel;
   float carbIntake;
   float appropriateDose;
   float iob;
   float overrideCorrectionFactor = 0.0f;
   int extendedDurationHours = 0;

   // tracking bolus state
   enum BolusState {
       ACTIVE,
       PAUSED,
       CANCELED
   };
   BolusState deliveryState = PAUSED;

   Profile* patientProfile;
};

#endif // BOLUS_H
