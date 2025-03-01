#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    : _loop(nullptr)
    , _exit(false)
    , _thread(std::bind(&EventLoopThread::threadFunc, this), name)
    , _cb(cb)
{
}

EventLoopThread::~EventLoopThread() {
    _exit = true;
    if(_loop != nullptr) {
        _loop->quit();
        _thread.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    _thread.start();
    EventLoop *loop = nullptr;
    {
        std::unique_lock<mutex> lck(_mtx);
        while(_loop != nullptr) {
            _cv.wait(lck);
        }
        loop = _loop;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if(_cb)
        _cb(&loop);
    {
        std::unique_lock<mutex> lck(_mtx);
        _loop = &loop;
        _cv.notify_one();
    }
    loop.loop();
    std::unique_lock<mutex> lck(_mtx);
    _loop = nullptr;
}