#ifndef _JSON_H_
#define _JSON_H_

#include <iostream>
#include <jsoncpp/json/json.h>
#include <map>
#include <fstream>
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

    enum
    {
        K_V = 0,
        V
    };

    enum
    {
        config = 0,
        normal
    };

private:
    Value _root;
    ofstream _ofs;
};

#endif

