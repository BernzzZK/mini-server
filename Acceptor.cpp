#include "EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <unistd.h>

static int createNoneBlocking() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0){
        std::ostringstream oss;
        oss << __FILE__ << ":" << __func__ << ":" << __LINE__<< " listen socket fail: " << errno;
        Logger::getInstance().logFATAL(oss.str());
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : _baseloop(loop)
    , _acceptSocket(createNoneBlocking())
    , _acceptChannel(loop, _acceptSocket.fd())
    , _listenning(false)
{
    _acceptSocket.setReuseAddr(true);
    _acceptSocket.setReusePort(true);
    _acceptSocket.bindAddr(listenAddr);
    _acceptChannel.setReadCallback(bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    _acceptChannel.disableAll();
    _acceptChannel.remove();
}

void Acceptor::listen() {
    _listenning = true;
    _acceptSocket.listen();
    _acceptChannel.enableReading();
}

void Acceptor::handleRead() {
    InetAddress peerAddr;
    int conn = _acceptSocket.accept(&peerAddr);
    if(conn > 0) {
        if(_newConnectionCallback)
            _newConnectionCallback(conn, peerAddr);
        else
            ::close(conn);
    }
    else {
        std::ostringstream oss1;
        oss1 << __FILE__ << ":" << __func__ << ":" << __LINE__ << " accept error: " << errno;
        Logger::getInstance().logERROR(oss1.str());
        if (errno == EMFILE) {
            std::ostringstream oss2;
            oss2 << __FILE__ << ":" << __func__ << ":" << __LINE__ << " sockfd reached limit" ;
            Logger::getInstance().logERROR(oss2.str());
        }
    }
}