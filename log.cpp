#include "log.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

// Constructor
Log::Log() : output("") {
    // Initialize the log with a header
    updateTime();
    output += " - Log initialized\n";
}

// Append text to the log with a timestamp
void Log::appendText(string s) {
    updateTime();
    output += " - " + s + "\n";
    
    // Also print to console for debugging
    std::cout << "[LOG] " << s << std::endl;
}

// Update the time in the log
void Log::updateTime() {
    // Get current time
    time_t now = time(0);
    tm* localTime = localtime(&now);
    
    // Format time as [HH:MM:SS]
    std::stringstream ss;
    ss << "[" << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
       << std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
       << std::setfill('0') << std::setw(2) << localTime->tm_sec << "]";
    
    // Add to output
    output += ss.str();
}

// Get the full log content
string Log::getFullLog() const {
    return output;
}

// Get the list of log entries
vector<string> Log::getLogEntries() const {
    return logEntries;
}

// Clear the log
void Log::clearLog() {
    output = "";
    logEntries.clear();

    // Reinitialize
    updateTime();
    string initMessage = " - Log cleared and reinitialized\n";
    output += initMessage;
    logEntries.push_back(initMessage);
}

// Save the log to a file
bool Log::saveToFile(const string& filename) {
    try {
        std::ofstream logFile(filename);
        if (logFile.is_open()) {
            logFile << output;
            logFile.close();
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}