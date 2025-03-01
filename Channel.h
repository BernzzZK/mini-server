#pragma once
#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>
#include "nocopyable.h"
#include "Timestamp.h"
class EventLoop;

class Channel {
public:
    using EventCallback = std::function<void()>;

    Channel(EventLoop *loop, int fd);

    void handleEvent();

    void setReadCallback(const EventCallback &cb) { _readCallback = cb; }
    void setWriteCallback(const EventCallback &cb) { _writeCallback = cb; }
    void setErrorCallback(const EventCallback &cb) { _errorCallback = cb; }

    void enableRead() { _events |= kReadEvent; update(); }
    void enableWrite() { _events |= kWriteEvent; update(); }
    void disableRead() { _events &= ~kReadEvent; update(); }
    void disableAll() { _events = kNoneEvent; update(); }

    int fd() const { return _fd; }
    int events() { return _events; }
    void set_revents(int revt) { _revents = revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    int index() { return _index; }
    void set_index(int idx) { _index = idx; }

    EventLoop *owenerLoop() { return _loop; }

private:
    EventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;

    EventLoop *_loop;
    const int _fd;
    int _events;
    int _revents;
    int _index;

    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};

#endif