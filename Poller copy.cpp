#include "Poller.h"
#include "Logger.h"
#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>
#include <cassert>
#include <iostream>

Poller::Poller(EventLoop* loop)
    : _ownerLoop(loop)
    {
    }

// Poller::~Poller()
// {
// }

// Timestamp Poller::poll(int timeout, ChannelList* activeChannels) {
//     int numEvents = ::poll(&*_pollfds.begin(), _pollfds.size(), timeout);
//     Timestamp now(Timestamp::now());
//     if (numEvents > 0) {
//         Logger::getInstance().logINFO(to_string(numEvents) + " events happended");
//         fillActiveChannels(numEvents, activeChannels);
//     } else if (numEvents == 0) {
//         Logger::getInstance().logINFO("nothing happended");
//     } else {
//         Logger::getInstance().logERROR("Poller::poll()");
//     }
//     return now;
// }

// void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
//     for (PollFdList::const_iterator pfd = _pollfds.begin(); pfd != _pollfds.end() && numEvents > 0; ++pfd) {
//         if(pfd->revents > 0) {
//             --numEvents;
//             ChannelMap::const_iterator ci = _channels.find(pfd->fd);
//             assert(ci != _channels.end());
//             Channel *channel = ci->second;
//             assert(channel->fd() == pfd->fd);
//             channel->set_revents(pfd->revents);
//             activeChannels->emplace_back(channel);
//         }
//     }
// }

// void Poller::updateChannel(Channel* channel) {
//     assertInLoopThread();
//     Logger::getInstance().logINFO("fd = " + to_string(channel->fd()) + " events = " + to_string(channel->events()));
//     if(channel->index() < 0) {
//         assert(_channels.find(channel->fd()) == _channels.end());
//         struct pollfd pfd;
//         pfd.events = static_cast<short>(channel->events());
//         pfd.revents = 0;
//         _pollfds.emplace_back(pfd);
//         int idx = _pollfds.size() - 1;
//         channel->set_index(idx);
//         _channels[pfd.fd] = channel;
//     } else {
//         assert(_channels.find(channel->fd()) != _channels.end());
//         assert(_channels[channel->fd()] == channel);
//         int idx = channel->index();
//         assert(0 <= idx && idx < static_cast<int>(_pollfds.size()));
//         struct pollfd &pfd = _pollfds[idx];
//         assert(pfd.fd == channel->fd() || pfd.fd == -1);
//         pfd.events = static_cast<short>(channel->events());
//         pfd.revents = 0;
//         if(channel->isNoneEvent()) {
//             pfd.fd = -1;
//         }
//     }
// }

void Poller::assertInLoopThread() {
     _ownerLoop->isInLoopThread();
}

// bool Poller::hasChannel(Channel *channel) const {
//     auto it = _channels.find(channel->fd());
//     return it != _channels.end() && it->second == channel;
// }