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

int createEventfd(){
    int evfd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (evfd < 0) {
        Logger::getInstance().logFATAL("eventfd fail: " + to_string(errno));
    }
    return evfd;
}

EventLoop::EventLoop() 
    : _looping(false)
    , _epoller(new EPoller(this))
    , _tid(CurrentThread::tid())
    , _callPendingFunctors(false)
    , _wakeupfd(createEventfd())
    , _wakeupChannel(new Channel(this, _wakeupfd))
{
    Logger::getInstance().logINFO("EventLoop created  in thread" + to_string(_tid));
    if(_loopInThisThread) {
        Logger::getInstance().logFATAL("Another EventLoop exists in this thread" + to_string(_tid));
    } else {
        _loopInThisThread = this;
    }
    _wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
    assert(!_looping);
    _wakeupChannel->disableAll();
    _wakeupChannel->remove();
    ::close(_wakeupfd);
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
            (*it)->handleEvent(_pollReturnTime);
        }
        doPendingFunctors();
    }

    Logger::getInstance().logINFO("EventLoop stop looping");
    _looping = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    _callPendingFunctors = true;
    {
        std::unique_lock<mutex> lck(_mtx);
        functors.swap(_pendingFunctors);
    }
    for(const Functor &functor : functors)
        functor();
    _callPendingFunctors = false;
}

void EventLoop::wakeup() {
    uint16_t one = 1;
    ssize_t n = write(_wakeupfd, &one, sizeof(one));
    if (n != sizeof(one))
        Logger::getInstance().logERROR("EventLoop::wakeup() writes" + to_string(n) + "bytes instead of 8");
}

void EventLoop::quit() {
    _quit = true;
    if(!isInLoopThread())
        wakeup();
}

void EventLoop::handleRead() {
    uint16_t one = 1;
    ssize_t n = read(_wakeupfd, &one, sizeof(one));
    if(n != sizeof(one))
        Logger::getInstance().logERROR("EventLoop::handleRead() reads" + to_string(n) + "bytes instead of 8");
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->owenerLoop() == this);
    assertInLoopThread();
    _epoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    _epoller->removeChannel(channel);
}

void EventLoop::hasChannel(Channel *channel) {
    _epoller->hasChannel(channel);
}

void EventLoop::runInLoop(Functor cb) {
    if(isInLoopThread())
        cb();
    else
        queueInLoop(cb);
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lck(_mtx);
        _pendingFunctors.emplace_back(cb);
    }
    if(!isInLoopThread() || _callPendingFunctors) {
        wakeup();
    }
}

#if 0
//test
#include <sys/timerfd.h>
#include <string.h>
EventLoop *g_loop;

void threadFunc()
{
    cout << "threadFunc(): pid = " << getpid() << ", tid = " << CurrentThread::tid() << endl;
    EventLoop loop;
    loop.loop();
}

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
#endif