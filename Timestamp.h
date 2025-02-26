#pragma once

#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_
#include <iostream>
#include <string>
using namespace std;
class Timestamp {
public:
    Timestamp();
    Timestamp(int64_t microSecond);
    static Timestamp now();
    string to_string();
    
private:
    int64_t _microSecondSinceEpoch;
};

#endif