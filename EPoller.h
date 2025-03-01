#pragma once 

#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include "Poller.h"
#include <sys/epoll.h>
#include <vector>
#include "Timestamp.h"

class Channel;

class EPoller : public Poller {
public:
    EPoller(EventLoop* loop);
    ~EPoller() override;

    Timestamp poll(int timeout, ChannelList *activechannel) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *activechannel) override;

private:

    static const int kInitEventsListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList *activechannel) const;
    void update(int operation, Channel *channel);

    using EventList = vector<epoll_event>;

    int _epollfd;
    EventList _events;
};

#endif