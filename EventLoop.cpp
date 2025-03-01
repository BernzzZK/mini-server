#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>
#include <cassert>
#include "EventLoop.h"
#include "Logger.h"
#include "Channel.h"
#include "EPoller.h"

__thread EventLoop *_loopInThisThread = 0;
const int kPollTimeMs = 10000;

EventLoop::EventLoop() 
    : _looping(false)
    , _epoller(new EPoller(this))
    , _tid(CurrentThread::tid())
{
    Logger::getInstance().logINFO("EventLoop created  in thread" + to_string(_tid));
    if(_loopInThisThread) {
        Logger::getInstance().logFATAL("Another EventLoop exists in this thread" + to_string(_tid));
    } else {
        _loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!_looping);
    _loopInThisThread = NULL;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread() {
    return _loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    Logger::getInstance().logFATAL("EventLoop::abortNotInLoopThrea was created in _tid : " + to_string(_tid) + ", current thread id : " + to_string(CurrentThread::tid()));
}

void EventLoop::loop() {
    assert(!_looping);
    assertInLoopThread();
    _looping = true;
    _quit = false;
    while(!_quit) {
        _activeChannels.clear();
        _epoller->poll(kPollTimeMs, &_activeChannels);
        for (ChannelList::iterator it = _activeChannels.begin(); it != _activeChannels.end(); ++it) {
            (*it)->handleEvent();
        }
    }

    Logger::getInstance().logINFO("EventLoop stop looping");
    _looping = false;
}

void threadFunc() {
    cout << "threadFunc(): pid = " << getpid() << ", tid = " << CurrentThread::tid() << endl;
    EventLoop loop;
    loop.loop();
}

void EventLoop::quit() {
    _quit = true;
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->owenerLoop() == this);
    assertInLoopThread();
    _epoller->updateChannel(channel);
}

//test
#include <sys/timerfd.h>
#include <string.h>
EventLoop *g_loop;

void timeout() {
    cout << "Timeout!" << endl;
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableRead();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 1;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);
    loop.loop();
    ::close(timerfd);
}