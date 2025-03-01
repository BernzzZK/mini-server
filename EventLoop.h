#pragma once
#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "nocopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;
class EPoller;

class EventLoop : mini_server::nocopyable {
public:
    using ChannelList = std::vector<Channel *>;

    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread() {
        if (!isInLoopThread())
            abortNotInLoopThread();
    }

    bool isInLoopThread() const { return _tid == CurrentThread::tid(); }
    EventLoop *getEventLoopOfCurrentThread();

    void updateChannel(Channel *channel);

    void quit();

private:
    void abortNotInLoopThread();
    
    std::atomic_bool _looping;
    std::atomic_bool _quit;
    std::unique_ptr<EPoller> _epoller;
    ChannelList _activeChannels;
    const std::atomic_int _tid;
};

#endif

