#pragma once
#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include "nocopyable.h"
#include "InetAddress.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class EventLoop;
class Channel;
class Acceptor;
class Socket;


class TcpConnection : mini_server::nocopyable , public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &lcaddr, const InetAddress &praddr);
    ~TcpConnection();

    EventLoop *getLoop() { return _baseloop; }
    const std::string &name() const { return _name; }
    const InetAddress &localAddress() const { return _lcaddr; }
    const InetAddress &peerAddress() const { return _praddr; }

    bool conneted() { return _status == kConnected; }

    void sent(const std::string &buff);
    void shutDown();

    void setConnectionCallback(const ConnectionCallback &cb)
    {
        _connectionCallback = cb;
    }
    void setMessageCallback(const MessageCallback &cb)
    {
        _messageCallback = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        _writeCompleteCallback = cb;
    }
    void setCloseCallback(const CloseCallback &cb)
    {
        _closeCallback = cb;
    }
    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark)
    {
        _highWaterMarkCallback = cb;
        _highWaterMark = highWaterMark;
    }

    void connectEstablished();
    void connectDestroyed();

private:
    enum Status
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };

    void setStatus(Status status) { _status = status; }

    void handleRead(Timestamp rectime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sentInLoop(const void *data, size_t len);
    void shutDownInLoop();

    EventLoop *_baseloop;
    std::string _name;
    InetAddress _lcaddr;
    InetAddress _praddr;
    std::atomic_int _status;
    bool _reading;

    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;

    ConnectionCallback _connectionCallback;       
    MessageCallback _messageCallback;             
    WriteCompleteCallback _writeCompleteCallback; 
    HighWaterMarkCallback _highWaterMarkCallback;
    CloseCallback _closeCallback;
    size_t _highWaterMark;
    
    Buffer _inputBuffer;
    Buffer _outputBuffer;
};

#endif