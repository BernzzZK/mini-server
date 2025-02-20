#include "redis.h"

bool redisConnection::setCommand(char* s, long long v) {
    if(isFree())
        return false;
    void *tmp = redisCommand(this->_connect, "set %s %ld", s, v);
    _result = (redisReply *)tmp;
    if(_result->type != 5) {
        return false;
    }
    freeReplyObject(this->_result);
    return true;
}

bool redisConnection::setCommand(long long v, char* s) {
    if (isFree())
        return false;
    void *tmp = redisCommand(this->_connect, "set %ld %s", v, s);
    _result = (redisReply *)tmp;
    if (_result->type != 5) {
        return false;
    }
    freeReplyObject(this->_result);
    return true;
}

bool redisConnection::setCommand(char* s1, char* s2) {
    if (isFree())
        return false;
    void *tmp = redisCommand(this->_connect, "set %s %s", s1, s2);
    _result = (redisReply *)tmp;
    if (_result->type != 5) {
        return false;
    }
    freeReplyObject(this->_result);
    return true;
}

bool redisConnection::setCommand(long long v1, long long v2) {
    if (isFree())
        return false;
    void *tmp = redisCommand(this->_connect, "set %ld %ld", v1, v2);
    _result = (redisReply *)tmp;
    if (_result->type != 5) {
        return false;
    }
    freeReplyObject(this->_result);
    return true;
}

string redisConnection::getIntValue(char* key) {
    if (isFree())
        return nullptr;
    void *tmp = redisCommand(this->_connect, "get %s", key);
    _result = (redisReply *)tmp;
    if (_result->type != 1)
    {
        return nullptr;
    }
    string value = _result->str;
    freeReplyObject(this->_result);
    return value;
}

string redisConnection::getIntValue(long long key) {
    if (isFree())
        return nullptr;
    void *tmp = redisCommand(this->_connect, "get %ld", key);
    _result = (redisReply *)tmp;
    if (_result->type != 1)
    {
        return nullptr;
    }
    string value = _result->str;
    freeReplyObject(this->_result);
    return value;
}

string redisConnection::getStringValue(char* key) {
    if (isFree())
        return nullptr;
    void *tmp = redisCommand(this->_connect, "get %s", key);
    _result = (redisReply *)tmp;
    if (_result->type != 1) {
        return nullptr;
    }
    string value = _result->str;
    freeReplyObject(this->_result);
    return value;
}

string redisConnection::getStringValue(long long key) {
    if(isFree())
        return nullptr;
    void *tmp = redisCommand(this->_connect, "get %ld", key);
    _result = (redisReply *)tmp;
    if (_result->type != 1) {
        return nullptr;
    }
    string value = _result->str;
    freeReplyObject(this->_result);
    return value;
}

vector<char*> redisConnection::keys() {
    if(isFree())
        return vector<char*>();
    void *tmp = redisCommand(this->_connect, "keys *");
    _result = (redisReply *)tmp;
    if(_result->type == 4) {
        return vector<char*>();
    }
    vector<char*> result;
    for (int i = 0; i < _result->elements; i++) {
        result.push_back(_result->element[i]->str);
    }
    return result;
}

redisContext* redisConnection::getContext() {
    return this->_connect;
}

bool redisConnection::isFree() {
    if(this->_connect->err != 0)
        return true;
    else
        return false;
}
