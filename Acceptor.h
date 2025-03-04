#pragma once
#ifndef _ACCEPT_H_
#define _ACCEPT_H_

#include "nocopyable.h"
#include <functional>
#include "Socket.h"
#include "Channel.h"

class EventLoop;
class InetAddress;

class Acceptor : mini_server::nocopyable {
public:
    using NewConnectionCallback = std::function<void(int, InetAddress)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) { _newConnectionCallback = cb; }
    bool listenning() const { return _listenning; }
    void listen();

private:
    void handleRead();

    EventLoop *_baseloop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    NewConnectionCallback _newConnectionCallback;
    bool _listenning;
};

#endif