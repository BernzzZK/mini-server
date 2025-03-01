#include "Logger.h"

Logger &Logger::getInstance() {
    static Logger log;
    return log;
}

/**
 * @brief if set openDebug true, it will print debug information once
 * 
 * @param openDebug whether open debug information
 * @return Logger& 
 */
Logger &Logger::getInstance(bool openDebug) {
    static Logger log;
    return log;
}

void Logger::logINFO(const string& msg) {
    string logLev = "[INFO]";
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if(_ofs.is_open())
        _ofs << logmsg << "\n";
}

void Logger::logFATAL(const string &msg) {
    string logLev = "[FATAL]";
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if (_ofs.is_open())
        _ofs << logmsg << "\n";
    exit(-1);
}

void Logger::logERROR(const string &msg) {
    string logLev = "[ERROR]";
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if (_ofs.is_open())
        _ofs << logmsg << "\n";
}

void Logger::logDEBUG(const string &msg) {
    if(_openDebug) {
        string logLev = "[DEBUG]";
        string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
        cout << logmsg << endl;
        if (_ofs.is_open())
            _ofs << logmsg << "\n";
    }
}

/**
 * @brief set logfile, all log will oupto the logfile
 * 
 * @param filePath 
 */
void Logger::setLogFile(const string &filePath) {
    _ofs.open(filePath, ios_base::app);
}

// int main() {
//     //test without logfile
//     // Logger::getInstance(true, "logfile.log").logINFO("this is a logINFO");
//     // Logger::getInstance().logERROR("this is a logERROR");
//     // Logger::getInstance(true, "").logDEBUG("this is a logDEBUG");
//     // Logger::getInstance().logFATAL("this is a logFATAL");

//     //test with logfile(if you want to use logfile, you need create a reference to logger)
//     // Logger& log = Logger::getInstance();
//     // log.setLogFile("logfile.log");
//     // Logger::getInstance().logINFO("this is a logINFO");
//     // Logger::getInstance().logERROR("this is a logERROR");
//     // Logger::getInstance().logDEBUG("this is a logDEBUG");
//     // Logger::getInstance().logFATAL("this is a logFATAL");

//     //if logFATAL() is trigered, prograss will exit
//     //Logger::getInstance(true, "logfile.log").logDEBUG("test debug will print or not");
//     return 0;
// }
