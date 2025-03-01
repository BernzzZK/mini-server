#pragma once 
#ifndef _EVENTLOOP_THREAD_H
#define _EVENTLOOP_THREAD_H

#include <mutex>
#include <condition_variable>
#include "nocopyable.h"
#include "Thread.h"

class EventLoop;

class EventLoopThread : mini_server::nocopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;
    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = std::string());
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();

    EventLoop *_loop;
    bool _exit;
    Thread _thread;
    std::mutex _mtx;
    std::condition_variable _cv;
    ThreadInitCallback _cb;
};

#endif