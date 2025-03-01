#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop,const std::string& name)
    : _baseLoop(baseLoop)
    , _name(name)
    , _started(false)
    , _threadNum(0)
    , _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    _started = true;
    for (int i = 0; i < _threadNum; ++i) {
        char buff[_name.size() + 32];
        snprintf(buff, sizeof(buff), "%s%d", _name.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buff);
        _threads.emplace_back(std::unique_ptr<EventLoopThread>(t));
        _loops.emplace_back(t->startLoop());
    }
    if (_threadNum == 0 && cb) {
        cb(_baseLoop);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *loop = _baseLoop;
    if(!_loops.empty()) {
        loop = _loops[_next];
        ++_next;
        if(_next  >= _loops.size())
            _next = 0;
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    if(!_loops.empty())
        return _loops;
    else
        return std::vector<EventLoop *>(1, _baseLoop);
}
