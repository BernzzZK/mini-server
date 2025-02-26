#include "logger.h"

Logger &Logger::getInstance() {
    static Logger log;
    return log;
}

void Logger::logINFO(const string& msg) {
    string logLev = "[INFO]";
    lock_guard<mutex> lck(_mutex);
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if(_ofs.is_open())
        _ofs << logmsg << "\n";
}

void Logger::logFATAL(const string &msg) {
    string logLev = "[FATAL]";
    lock_guard<mutex> lck(_mutex);
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if (_ofs.is_open())
        _ofs << logmsg << "\n";
    exit(-1);
}

void Logger::logERROR(const string &msg) {
    string logLev = "[ERROR]";
    lock_guard<mutex> lck(_mutex);
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if (_ofs.is_open())
        _ofs << logmsg << "\n";
}

void Logger::logDEBUG(const string &msg) {
    string logLev = "[DEBUG]";
    lock_guard<mutex> lck(_mutex);
    string logmsg = Timestamp::now().to_string() + " " + logLev + ": " + msg + ".";
    cout << logmsg << endl;
    if (_ofs.is_open())
        _ofs << logmsg << "\n";
}

void Logger::setLogFile(const string &filePath) {
    _ofs.open(filePath, ios_base::app);
}

// int main() {
//     //test without logfile
//     // Logger::getInstance().logINFO("this is a logINFO");
//     // Logger::getInstance().logERROR("this is a logERROR");
//     // Logger::getInstance().logDEBUG("this is a logDEBUG");
//     // Logger::getInstance().logFATAL("this is a logFATAL");

//     //test with logfile(if you want to use logfile, you need create a reference to logger)
//     // Logger& log = Logger::getInstance();
//     // log.setLogFile("logfile.log");
//     // Logger::getInstance().logINFO("this is a logINFO");
//     // Logger::getInstance().logERROR("this is a logERROR");
//     // Logger::getInstance().logDEBUG("this is a logDEBUG");
//     // Logger::getInstance().logFATAL("this is a logFATAL");

//     //if logFATAL() is trigered, prograss will exit
//     Logger &log = Logger::getInstance();
//     log.setLogFile("logfile.log");
//     Logger::getInstance().logFATAL("this will make prograss exit!");
//     return 0;
// }
