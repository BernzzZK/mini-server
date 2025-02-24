#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <hiredis/hiredis.h>
#include <string>
#include <vector>
using namespace std;

/// @brief 
class redisConnection {
public:
    redisConnection()
    : _ip("127.0.0.1")
    , _port(6379)
    //, _tv()
    {
        _connect = redisConnect(_ip, _port);
        if(_connect->err != 0){
            cerr << "connect fail" << endl;
        } 
    }

    redisConnection(char* IP, int PORT)
    : _ip(IP)
    , _port(PORT)
    {
        _connect = redisConnect(_ip, _port);
        if (_connect->err != 0)
        {
            cerr << "connect fail" << endl;
        }
    }

    bool setCommand(char* s, long long v);
    bool setCommand(char* s1, char* s2);
    bool setCommand(long long v, char* s);
    bool setCommand(long long v1, long long v2);


    long long getIntValue(char* key);
    long long getIntValue(long long key);
    string getStringValue(char* key);
    string getStringValue(long long key);

    vector<char*> keys();

    redisContext *getContext();

    /**
     * @brief just excute cmd, if succeed return true, otherwise return false
     * 
     * @param cmd 
     * @return true 
     * @return false 
     */
    bool command(char* cmd);

    ~redisConnection() {
        if(!isFree())
            redisFree(this->_connect);
    }

private:
    redisContext *_connect;
    redisReply *_result;
    char *_ip;
    int _port;
    struct timeval _tv;
    bool isFree();
};

#endif