#pragma once
#ifndef _TCPServer_H_
#define _TCPServer_H_

#include <functional>
#include <string>
#include "Callbacks.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include <sstream>
#include "EventLoopThreadPool.h"
#include "Channel.h"
#include <string.h>
#include "TcpConnection.h"

class TcpServer
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;
    enum Options
    {
        kNoReusePort,
        kReusePort
    };

    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &name, Options opt = kNoReusePort);
    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback &cb) { _threadInitCallback = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback = cb; }

    void setNumThread(int numThread);

    void start();

private:
    void newConnection(int sockfd, const InetAddress &praddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using Connections = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *_baseloop;
    const std::string _IpPort;
    const std::string _name;
    std::unique_ptr<Acceptor> _acc;
    std::unique_ptr<EventLoopThreadPool> _threadPool;

    ThreadInitCallback _threadInitCallback;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;

    std::atomic_int _started;
    int _nextConnId;
    Connections _connections;
};

#endif