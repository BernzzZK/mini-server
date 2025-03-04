#include "EPoller.h"
#include "Logger.h"
#include "Channel.h"
#include <unistd.h>
#include <memory.h>

const int kNew = 0;
const int kAdded = 1;
const int kDelete = 2;

EPoller::EPoller(EventLoop* loop)
    : Poller(loop)
    , _epollfd(::epoll_create1(EPOLL_CLOEXEC))
    , _events(kInitEventsListSize)
    {
        if(_epollfd < 0) {
            Logger::getInstance().logFATAL("epoll_create error " + to_string(errno));
        }
    }

EPoller::~EPoller() {
    ::close(_epollfd);
}

Timestamp EPoller::poll(int timeout, ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(_epollfd, &*_events.begin(), _events.size(), timeout);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        Logger::getInstance().logINFO(to_string(numEvents) + " events happended");
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == _events.size())
            _events.resize(_events.size() * 2);
    }
    else if (numEvents == 0) {
        Logger::getInstance().logINFO("nothing happended");
    } else {
        Logger::getInstance().logERROR("Poller::poll()");
    }
    return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (EventList::const_iterator pfd = _events.begin(); pfd != _events.end() && numEvents > 0; ++pfd) {
        Channel *channel = static_cast<Channel *>(pfd->data.ptr);
        channel->set_revents(pfd->events);
        activeChannels->emplace_back(channel);
    }
}

void EPoller::updateChannel(Channel* channel) {
    const int idx = channel->index();
    if(channel->index() == kNew || channel->index() == kDelete) {
        if(channel->index() == kNew) {
            int fd = channel->fd();
            _channels[fd] = channel;
        }
        else {}
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        if(channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDelete);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    _channels.erase(fd);
    int idx = channel->index();
    if(idx == kAdded) {
        update(EPOLL_CTL_DEL, channel);
        channel->set_index(kDelete);    
    }
    channel->set_index(kNew);
}

void EPoller::update(int operation, Channel *channel) {
    epoll_event event;
    ::memset(&event, 0, sizeof(event));
    int fd = channel->fd();
    event.data.fd = fd;
    event.data.ptr = channel;
    if(::epoll_ctl(_epollfd, operation, fd, &event)) {
        if(operation == EPOLL_CTL_DEL)
            Logger::getInstance().logERROR("epoll_ctl del error: " + to_string(errno));
        else
            Logger::getInstance().logFATAL("epoll_ctl add/mod error: " + to_string(errno));
    }
}

bool EPoller::hasChannel(Channel *channel) {
    auto it = _channels.find(channel->fd());
    return it != _channels.end() && it->second == channel;
}