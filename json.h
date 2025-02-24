#ifndef _JSON_H_
#define _JSON_H_

#include <iostream>
#include <jsoncpp/json/json.h>
#include <cassert>
#include <fstream>
#include <vector>
using namespace std;
using namespace Json;

class jsoncpp {
public:
    jsoncpp() {};
    ~jsoncpp() {};
    template <typename T>
    void jsonAppend(string key, T value) {
        _root[key] = value;
    };
    void jsonAppend(Value& value);
    
    void saveJson(string filename, int type);
    void readJson(string filename);

    Value getRoot();

    enum
    {
        config = 0,
        normal
    };

private:
    Value _root;
    Reader _read;
    ofstream _ofs;
    ifstream _ifs;
};

#endif

