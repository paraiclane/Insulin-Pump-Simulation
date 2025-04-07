#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <iostream>

using namespace std;

class Profile {
    private:
        string mode;
        float basalRate;
        float correctionFactor;
        float carbohydratesRatio;
        float targetGlucoseLevels;
        bool selectedProfile;

    public:
        // Constructor & Destructor
        Profile(const string& mode, float basalRate, float correctionFactor,
                float carbohydratesRatio, float targetGlucoseLevels);
        ~Profile();

        // Getters
        string getMode() const { return mode; }
        float getBasalRate() const { return basalRate; }
        float getCorrectionFactor() const { return correctionFactor; }
        float getCarbohydratesRatio() const { return carbohydratesRatio; }
        float getTargetGlucoseLevels() const { return targetGlucoseLevels; }
        bool isSelected() const { return selectedProfile; }

        // Setters
        void setMode(const string& mode) { this->mode = mode; }
        void setBasalRate(float basalRate) { this->basalRate = basalRate; }
        void setCorrectionFactor(float correctionFactor) { this->correctionFactor = correctionFactor; }
        void setCarbohydratesRatio(float carbohydratesRatio) { this->carbohydratesRatio = carbohydratesRatio; }
        void setTargetGlucoseLevels(float targetGlucoseLevels) { this->targetGlucoseLevels = targetGlucoseLevels; }
        void setSelectedProfile(bool selectedProfile) { this->selectedProfile = selectedProfile; }

        // Print and Debug
        void print();
        string toString() const;
};

#endif // PROFILE_H
