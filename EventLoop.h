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
    using Functor = function<void()>;

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
    void removeChannel(Channel *channnel);
    void hasChannel(Channel *channel);

    void quit();
    void wakeup();

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    Timestamp pollReturnTime() const { return _pollReturnTime; }

private:
    void abortNotInLoopThread();
    void doPendingFunctors();
    void handleRead();

    Timestamp _pollReturnTime;
    std::atomic_bool _looping;
    std::atomic_bool _quit;
    std::atomic_bool _callPendingFunctors;
    std::unique_ptr<EPoller> _epoller;
    ChannelList _activeChannels;
    const std::atomic_int _tid;
    vector<Functor> _pendingFunctors;
    mutex _mtx;
    int _wakeupfd;
    std::unique_ptr<Channel> _wakeupChannel;
};

#endif