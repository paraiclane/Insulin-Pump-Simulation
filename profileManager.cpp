#include "profileManager.h"
#include <iostream>
#include <sstream>

ProfileManager::ProfileManager() : currProfile(nullptr) {}

ProfileManager::~ProfileManager() {
    for (Profile* p : profileList) {
        delete p;
    }
}

// Check if a profile with the same name already exists
bool ProfileManager::checkDuplicateName(const string& mode) {
    return searchName(mode) != -1;
}

// Validate profile parameters
bool ProfileManager::validateProfileParams(const string& mode, float basalRate, float correctionFactor,
                                         float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg) {
    // Validate mode name
    if (mode.empty()) {
        errorMsg = "Mode name cannot be empty";
        return false;
    }

    // Validate basal rate
    static const float MIN_BASAL_RATE = 0.0f;
    static const float MAX_BASAL_RATE = 30.0f;
    if (basalRate < MIN_BASAL_RATE || basalRate > MAX_BASAL_RATE) {
        errorMsg = "Basal rate must be between " + to_string(MIN_BASAL_RATE) +
                   " and " + to_string(MAX_BASAL_RATE);
        return false;
    }

    // Validate correction factor
    static const float MIN_CORRECTION_FACTOR = 1.0f;
    static const float MAX_CORRECTION_FACTOR = 400.0f;
    if (correctionFactor < MIN_CORRECTION_FACTOR || correctionFactor > MAX_CORRECTION_FACTOR) {
        errorMsg = "Correction factor must be between " + to_string(MIN_CORRECTION_FACTOR) +
                   " and " + to_string(MAX_CORRECTION_FACTOR);
        return false;
    }

    // Validate carbohydrate ratio
    static const float MIN_CARB_RATIO = 1.0f;
    static const float MAX_CARB_RATIO = 150.0f;
    if (carbohydratesRatio < MIN_CARB_RATIO || carbohydratesRatio > MAX_CARB_RATIO) {
        errorMsg = "Carbohydrate ratio must be between " + to_string(MIN_CARB_RATIO) +
                   " and " + to_string(MAX_CARB_RATIO);
        return false;
    }

    // Validate target glucose levels
    static const float MIN_TARGET_GLUCOSE = 70.0f;
    static const float MAX_TARGET_GLUCOSE = 180.0f;
    if (targetGlucoseLevels < MIN_TARGET_GLUCOSE || targetGlucoseLevels > MAX_TARGET_GLUCOSE) {
        errorMsg = "Target glucose levels must be between " + to_string(MIN_TARGET_GLUCOSE) +
                   " and " + to_string(MAX_TARGET_GLUCOSE);
        return false;
    }

    return true;
}

// Create a new profile
bool ProfileManager::createProfile(const string& mode, float basalRate, float correctionFactor,
                                 float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg) {
    // Check if the name already exists
    if (checkDuplicateName(mode)) {
        errorMsg = "Profile '" + mode + "' already exists";
        cout << errorMsg << endl;
        return false;
    }

    // Validate all parameters
    if (!validateProfileParams(mode, basalRate, correctionFactor, carbohydratesRatio,
                             targetGlucoseLevels, errorMsg)) {
        cout << "Failed to create profile: " << errorMsg << endl;
        return false;
    }

    try {
        // Create the profile
        Profile* p = new Profile(mode, basalRate, correctionFactor, carbohydratesRatio, targetGlucoseLevels);
        profileList.push_back(p);

        // Automatically activate if this is the first profile
        if (profileList.size() == 1) {
            currProfile = p;
        }

        cout << "Profile '" << mode << "' created successfully" << endl;
        return true;
    } catch (const std::exception& e) {
        errorMsg = "Error occurred while creating profile: " + string(e.what());
        cout << errorMsg << endl;
        return false;
    }
}

// Read a profile
Profile* ProfileManager::readProfile(const string& mode) {
    int index = searchName(mode);
    if (index != -1) {
        return profileList[index];
    } else {
        cout << "Profile with name '" << mode << "' not found" << endl;
        return nullptr;
    }
}

// Update a profile
bool ProfileManager::updateProfile(const string& mode, float basalRate, float correctionFactor,
                                 float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg) {
    // Find the profile
    int index = searchName(mode);
    if (index == -1) {
        errorMsg = "Profile with name '" + mode + "' not found";
        cout << errorMsg << endl;
        return false;
    }

    // Validate parameters
    if (!validateProfileParams(mode, basalRate, correctionFactor, carbohydratesRatio,
                             targetGlucoseLevels, errorMsg)) {
        cout << "Failed to update profile: " << errorMsg << endl;
        return false;
    }

    try {
        // Update the profile
        Profile* p = profileList[index];
        p->setBasalRate(basalRate);
        p->setCorrectionFactor(correctionFactor);
        p->setCarbohydratesRatio(carbohydratesRatio);
        p->setTargetGlucoseLevels(targetGlucoseLevels);

        cout << "Profile '" << mode << "' updated successfully" << endl;
        return true;
    } catch (const std::exception& e) {
        errorMsg = "Error occurred while updating profile: " + string(e.what());
        cout << errorMsg << endl;
        return false;
    }
}

// Delete a profile
bool ProfileManager::deleteProfile(const string& mode, string& errorMsg) {
    // Find the profile
    int index = searchName(mode);
    if (index == -1) {
        errorMsg = "Profile with name '" + mode + "' not found";
        cout << errorMsg << endl;
        return false;
    }

    // Check if it is the currently active profile
    if (profileList[index] == currProfile) {
        errorMsg = "Cannot delete the currently active profile";
        cout << errorMsg << endl;
        return false;
    }

    // Delete the profile
    delete profileList[index];
    profileList.erase(profileList.begin() + index);

    cout << "Profile '" << mode << "' deleted successfully" << endl;
    return true;
}

// Activate a profile
bool ProfileManager::activateProfile(const string& mode, string& errorMsg) {
    // Find the profile
    int index = searchName(mode);
    if (index == -1) {
        errorMsg = "Profile with name '" + mode + "' not found";
        cout << errorMsg << endl;
        return false;
    }

    // Set the current profile
    currProfile = profileList[index];

    cout << "Profile '" << mode << "' activated" << endl;
    return true;
}

// Check if a profile exists
bool ProfileManager::profileExists(const string& mode) const {
    for (const Profile* p : profileList) {
        if (p->getMode() == mode) {
            return true;
        }
    }
    return false;
}

// Search for a profile index
int ProfileManager::searchName(const string& mode) {
    for (size_t i = 0; i < profileList.size(); ++i) {
        if (profileList[i]->getMode() == mode) {
            return i;
        }
    }
    return -1;
}

// Validate all profiles
bool ProfileManager::validateAllProfiles(vector<string>& errorMessages) {
    bool allValid = true;

    for (size_t i = 0; i < profileList.size(); ++i) {
        Profile* p = profileList[i];

        // Validate basal rate
        static const float MIN_BASAL_RATE = 0.0f;
        static const float MAX_BASAL_RATE = 30.0f;
        if (p->getBasalRate() < MIN_BASAL_RATE || p->getBasalRate() > MAX_BASAL_RATE) {
            string error = "Profile '" + p->getMode() + "': Invalid basal rate";
            errorMessages.push_back(error);
            allValid = false;
        }

        // Validate correction factor
        static const float MIN_CORRECTION_FACTOR = 1.0f;
        static const float MAX_CORRECTION_FACTOR = 400.0f;
        if (p->getCorrectionFactor() < MIN_CORRECTION_FACTOR || p->getCorrectionFactor() > MAX_CORRECTION_FACTOR) {
            string error = "Profile '" + p->getMode() + "': Invalid correction factor";
            errorMessages.push_back(error);
            allValid = false;
        }

        // Validate carbohydrate ratio
        static const float MIN_CARB_RATIO = 1.0f;
        static const float MAX_CARB_RATIO = 150.0f;
        if (p->getCarbohydratesRatio() < MIN_CARB_RATIO || p->getCarbohydratesRatio() > MAX_CARB_RATIO) {
            string error = "Profile '" + p->getMode() + "': Invalid carbohydrate ratio";
            errorMessages.push_back(error);
            allValid = false;
        }

        // Validate target glucose levels
        static const float MIN_TARGET_GLUCOSE = 70.0f;
        static const float MAX_TARGET_GLUCOSE = 180.0f;
        if (p->getTargetGlucoseLevels() < MIN_TARGET_GLUCOSE || p->getTargetGlucoseLevels() > MAX_TARGET_GLUCOSE) {
            string error = "Profile '" + p->getMode() + "': Invalid target glucose levels";
            errorMessages.push_back(error);
            allValid = false;
        }
    }

    return allValid;
}
