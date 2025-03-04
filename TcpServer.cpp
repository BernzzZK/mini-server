#include "TcpServer.h"
#include "Logger.h"

EventLoop *checkLoopNotNone(EventLoop *loop)
{
    if (loop == nullptr)
    {
        std::ostringstream oss;
        oss << __FILE__ << ":" << __func__ << ":" << __LINE__ << " main loop is null!";
        Logger::getInstance().logFATAL(oss.str());
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &name, Options opt)
    : _baseloop(checkLoopNotNone(loop))
    , _IpPort(listenAddr.toIPPort())
    , _name(name)
    , _acc(new Acceptor(loop, listenAddr, opt == kReusePort))
    , _threadPool(new EventLoopThreadPool(loop, _name))
    , _connectionCallback()
    , _messageCallback()
    , _nextConnId(1)
    , _started(0)
{
    _acc->setNewConnectionCallback(bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (auto &item : _connections)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setNumThread(int numThread) {
    _threadPool->setThreadNum(numThread);
}

void TcpServer::start() {
    if (_started++ == 0)
    {
        _threadPool->start(_threadInitCallback);
        _baseloop->runInLoop(std::bind(&Acceptor::listen, _acc.get()));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {

    EventLoop *ioLoop = _threadPool->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", _IpPort.c_str(), _nextConnId);
    ++_nextConnId; 
    std::string connName = _name + buf;

    Logger::getInstance().logINFO("TcpServer::newConnection [" + _name + "] - new connection [" + connName + "] from" + peerAddr.toIPPort());

    sockaddr_in local;
    ::memset(&local, 0, sizeof(local));
    socklen_t addrlen = sizeof(local);
    if (::getsockname(sockfd, (sockaddr *)&local, &addrlen) < 0)
    {
        Logger::getInstance().logERROR("sockets::getLocalAddr");
    }

    InetAddress localAddr(local);
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);

    // 设置了如何关闭连接的回调
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioLoop->runInLoop(
        std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    _baseloop->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    Logger::getInstance().logINFO("TcpServer::removeConnectionInLoop [" + _name + "] - connection" + conn->name());
    _connections.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
}
