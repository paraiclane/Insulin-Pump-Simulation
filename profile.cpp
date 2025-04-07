#include "profile.h"
#include <sstream>

// Modify the constructor to accept a const string reference instead of string&, to be compatible with ProfileManager
Profile::Profile(const string& mode, float basalRate, float correctionFactor, float carbohydratesRatio, float targetGlucoseLevels) :
    mode(mode), basalRate(basalRate), correctionFactor(correctionFactor),
    carbohydratesRatio(carbohydratesRatio), targetGlucoseLevels(targetGlucoseLevels),
    selectedProfile(false) {

    // Validation is not performed in the constructor, but relies on ProfileManager's validation
}

Profile::~Profile() {}

// Print method to provide detailed configuration information
void Profile::print() {
    cout << "Profile: " << mode << endl;
    cout << "  Basal Rate: " << basalRate << endl;
    cout << "  Correction Factor: " << correctionFactor << endl;
    cout << "  Carbohydrates Ratio: " << carbohydratesRatio << endl;
    cout << "  Target Glucose Levels: " << targetGlucoseLevels << endl;
    cout << "  Status: " << (selectedProfile ? "Activated" : "Deactivated") << endl;
}

// Method to convert to string, convenient for logging and display
string Profile::toString() const {
    stringstream ss;
    ss << "Profile [Mode: " << mode
       << ", Basal Rate: " << basalRate
       << ", Correction Factor: " << correctionFactor
       << ", Carbohydrates Ratio: " << carbohydratesRatio
       << ", Target Glucose Levels: " << targetGlucoseLevels << "]";
    return ss.str();
}
