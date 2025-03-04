#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <netinet/tcp.h>

Socket::~Socket(){
    ::close(_sockfd);
}

void Socket::bindAddr(const InetAddress &localaddr) {
    if(0 != ::bind(_sockfd, (sockaddr*)localaddr.getAddr(),sizeof(sockaddr_in))) {
        Logger::getInstance().logFATAL("bind sockfd: %d fail" + to_string(_sockfd));
    }
}

void Socket::listen(){
    if(0!= ::listen(_sockfd, 1024)){
        Logger::getInstance().logFATAL("listen sockfd: %d fail" + to_string(_sockfd));
    }
}

int Socket::accept(InetAddress *peerAddr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr, len);
    int conn = ::accept4(_sockfd, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(conn >=0 ) {
        peerAddr->setAddr(addr);
    }
    return conn;
}

void Socket::shutDownWrite() {
    if(::shutdown(_sockfd, SHUT_WR) < 0) {
        Logger::getInstance().logERROR("shutdownwrite error");
    }
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}