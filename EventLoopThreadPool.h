#pragma once 

#ifndef _EVENTLOOPTHREADPOLL_H_
#define _EVENTLOOPTHREADPOLL_H_

#include "nocopyable.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : mini_server::nocopyable {
public:
    using ThreadInitCallback = std::function < void(EventLoop *)>;
    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int num) { _threadNum = num; }

    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    EventLoop *getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return _started; }
    const std::string name() const { return _name; }

private:
    EventLoop *_baseLoop;
    std::string _name;
    int _threadNum;
    bool _started;
    int _next;
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop *> _loops;
};

#endif