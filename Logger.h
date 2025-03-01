#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <fstream>
#include "Timestamp.h"
#include "nocopyable.h"

enum logLevel
{
    INFO, // normal infomation
    ERROR, // error infomation
    FATAL, //core dump
    DEBUG, //debug infomation
}; 

class Logger : mini_server::nocopyable {
public:
    static Logger &getInstance(bool openDebug);
    static Logger &getInstance();
    //void setLogLevel(int level);
    void logINFO(const string& msg);
    void logERROR(const string &msg);
    void logFATAL(const string &msg);
    void logDEBUG(const string &msg);

    void setLogFile(const string &filePath);

private:
    ofstream _ofs;
    bool _openDebug;
    string _logFile;
};

#endif