#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Log {
    private:
        string output;
    public:
        Log();

        void appendText(string s);
        void updateTime();
        vector<string> logEntries;

        //control log
        string getFullLog() const;
        void clearLog();
        bool saveToFile(const string& filename);
        vector<string> getLogEntries() const;
};

#endif // LOG_H

