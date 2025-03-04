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
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);

    void handleEvent(Timestamp time);

    void setReadCallback(const ReadEventCallback &cb) { _readCallback = cb; }
    void setWriteCallback(const EventCallback &cb) { _writeCallback = cb; }
    void setErrorCallback(const EventCallback &cb) { _errorCallback = cb; }
    void setCloseCallback(const EventCallback &cb) { _closeCallback = cb; }

    void enableReading()
    {
        _events |= kReadEvent;
        update();
    }
    void disableReading()
    {
        _events &= ~kReadEvent;
        update();
    }
    void enableWriting()
    {
        _events |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        _events &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        _events = kNoneEvent;
        update();
    }
    bool isWriting() const { return _events & kWriteEvent; }
    bool isReading() const { return _events & kReadEvent; }

    int fd() const { return _fd; }
    int events() { return _events; }
    void set_revents(int revt) { _revents = revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    int index() { return _index; }
    void set_index(int idx) { _index = idx; }
    void remove();  

    EventLoop *owenerLoop() { return _loop; }

    void tie(const std::shared_ptr<void> &obj);

private:
    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
    EventCallback _closeCallback;

    EventLoop *_loop;
    const int _fd;
    int _events;
    int _revents;
    int _index;

    std::weak_ptr<void> _tie;
    bool _tied;

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};

#endif