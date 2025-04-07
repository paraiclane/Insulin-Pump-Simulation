
#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <vector>
#include <string>
#include "profile.h"

class ProfileManager {
    private:
        vector<Profile*> profileList;

        // Internal helper functions for parameter validation and error checking
        bool validateProfileParams(const string& mode, float basalRate, float correctionFactor,
                                 float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg);
        bool checkDuplicateName(const string& mode);

    public:
        // Constructor & Destructor
        ProfileManager();
        ~ProfileManager();
        Profile* currProfile = nullptr;

        // Getter
        vector<Profile*> getProfileList() { return profileList; }

        // Enhanced CRUD operations - now return success status and provide error messages
        bool createProfile(const string& mode, float basalRate, float correctionFactor,
                         float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg);

        Profile* readProfile(const string& mode);

        bool updateProfile(const string& mode, float basalRate, float correctionFactor,
                         float carbohydratesRatio, float targetGlucoseLevels, string& errorMsg);

        bool deleteProfile(const string& mode, string& errorMsg);

        // Profile selection and activation
        bool activateProfile(const string& mode, string& errorMsg);
        Profile* getActiveProfile() const { return currProfile; }

        // Search and helper functions
        int searchName(const string& mode);
        int getProfileCount() const { return profileList.size(); }
        bool profileExists(const string& mode) const;

        // Validate all profiles - used for system startup checks
        bool validateAllProfiles(vector<string>& errorMessages);
};

#endif // PROFILEMANAGER_H
