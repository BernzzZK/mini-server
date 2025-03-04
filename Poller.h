#pragma once
#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <unordered_map>

#include "nocopyable.h"
#include "Timestamp.h"

class Channel;
class EventLoop;

class Poller : mini_server::nocopyable
{
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeout, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0 ;
    virtual void removeChannel(Channel *channel) = 0;

    //bool hasChannel(Channel* channel) const;


    void assertInLoopThread();

protected:
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap _channels;
    
    
private:
    //using PollFdList = std::vector<struct pollfd>;
    EventLoop *_ownerLoop;
    //PollFdList _pollfds;
    
    // void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
};

#endif