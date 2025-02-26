#pragma once 
#ifndef _NOCOPYABLE_H_
#define _NOCOPYABLE_H_

namespace mini_server {

class nocopyable{
public:
    nocopyable(const nocopyable&) = delete;
    nocopyable &operator=(const nocopyable &) = delete;
protected:
    nocopyable() = default;
    ~nocopyable() = default;
};
};

#endif