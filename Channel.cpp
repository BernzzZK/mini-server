#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

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

void Channel::handleEvent() {
    if (_revents & POLLNVAL)
        Logger::getInstance().logERROR("Channel::handle_evet() POLLNVAL");
    if (_revents & (POLLERR | POLLNVAL))
        if (_errorCallback)
            _errorCallback();
    if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
        if (_readCallback)
            _readCallback();
    if (_revents & POLLOUT)
        if (_writeCallback)
            _writeCallback();
}