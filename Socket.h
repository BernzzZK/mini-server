#pragma once
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "nocopyable.h"

class InetAddress;
class Socket : mini_server::nocopyable {
public:
    explicit Socket(int sockfd)
        : _sockfd(sockfd) 
    {}
    ~Socket();

    int fd() { return _sockfd; }
    void bindAddr(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutDownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    int _sockfd;
};

#endif