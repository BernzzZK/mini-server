#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : _loop(loop)
    , _fd(fd)
    , _events(0)
    , _revents(0)
    , _index(0)
    {
    }

void Channel::update() {
    _loop->updateChannel(this);
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    _tie = obj;
    _tied = true;
}

void Channel::handleEvent(Timestamp receiveTime)
{
    if (_tied)
    {
        std::shared_ptr<void> guard = _tie.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    Logger::getInstance().logINFO("channel handleEvent revents:" + to_string(_revents));
    if ((_revents & EPOLLHUP) && !(_revents & EPOLLIN))
    {
        if (_closeCallback)
        {
            _closeCallback();
        }
    }
    if (_revents & EPOLLERR)
    {
        if (_errorCallback)
        {
            _errorCallback();
        }
    }
    if (_revents & (EPOLLIN | EPOLLPRI))
    {
        if (_readCallback)
        {
            _readCallback(receiveTime);
        }
    }
    if (_revents & EPOLLOUT)
    {
        if (_writeCallback)
        {
            _writeCallback();
        }
    }
}

void Channel::remove() {
    _loop->removeChannel(this);
}