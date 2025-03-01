#pragma once 
#ifndef _THREAD_H_
#define _THREAD_H_

#include "nocopyable.h"
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <string>

class Thread : mini_server::nocopyable {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc threadFunc, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started() { return _start; };
    pid_t tid() { return _tid; }
    const std::string name() { return _name; };

private:
    bool _start;
    bool _join;
    pid_t _tid;
    std::unique_ptr<std::thread> _thread;
    std::string _name;
    ThreadFunc _threadFunc;
    static std::atomic_int _numCreated;

    void setDefaultName();
};

#endif

