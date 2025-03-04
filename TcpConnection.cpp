#include "TcpConnection.h"
#include "Logger.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include <sstream>

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        std::ostringstream oss;
        oss << __FILE__ << ":" << __func__ << ":" << __LINE__;
        Logger::getInstance().logFATAL(oss.str());
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop,
                             const std::string &name,
                             int sockfd,
                             const InetAddress &lcaddr,
                             const InetAddress &praddr)
    : _baseloop(CheckLoopNotNull(loop))
    , _name(name)
    , _status(kConnecting)
    , _reading(true)
    , _socket(new Socket(sockfd))
    , _channel(new Channel(loop, sockfd))
    , _lcaddr(lcaddr)
    , _praddr(praddr)
    , _highWaterMark(64 * 1024 * 1024)
{
    _channel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));
    Logger::getInstance().logINFO("TcpConnection::ctor[" + _name + "] at fd=" + to_string(sockfd));
    _socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    Logger::getInstance().logINFO("TcpConnection::dtor[" + _name + "] at fd=" + to_string(_channel->fd()) + "status=" + to_string((int)_status));
}

void TcpConnection::sent(const std::string &buf)
{
    if (_status == kConnected)
    {
        if (_baseloop->isInLoopThread())
        {
            sentInLoop(buf.c_str(), buf.size());
        }
        else
        {
            _baseloop->runInLoop(
                std::bind(&TcpConnection::sentInLoop, this, buf.c_str(), buf.size()));
        }
    }
}

void TcpConnection::sentInLoop(const void *data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if (_status == kDisconnected)
    {
        Logger::getInstance().logERROR("disconnected, give up writing");
    }

    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0)
    {
        nwrote = ::write(_channel->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && _writeCompleteCallback)
            {
                _baseloop->queueInLoop(
                    std::bind(_writeCompleteCallback, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                Logger::getInstance().logERROR("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET) 
                {
                    faultError = true;
                }
            }
        }
    }
    if (!faultError && remaining > 0)
    {
        size_t oldLen = _outputBuffer.readableBytes();
        if (oldLen + remaining >= _highWaterMark && oldLen < _highWaterMark && _highWaterMarkCallback)
        {
            _baseloop->queueInLoop(
                std::bind(_highWaterMarkCallback, shared_from_this(), oldLen + remaining));
        }
        _outputBuffer.append((char *)data + nwrote, remaining);
        if (!_channel->isWriting())
        {
            _channel->enableWriting(); // 这里一定要注册channel的写事件 否则poller不会给channel通知epollout
        }
    }
}

void TcpConnection::shutDown()
{
    if (_status == kConnected)
    {
        setStatus(kDisconnecting);
        _baseloop->runInLoop(
            std::bind(&TcpConnection::shutDownInLoop, this));
    }
}

void TcpConnection::shutDownInLoop()
{
    if (!_channel->isWriting())
    {
        _socket->shutDownWrite();
    }
}

// 连接建立
void TcpConnection::connectEstablished()
{
    setStatus(kConnected);
    _channel->tie(shared_from_this());
    _channel->enableReading(); 
    _connectionCallback(shared_from_this());
}
// 连接销毁
void TcpConnection::connectDestroyed()
{
    if (_status == kConnected)
    {
        setStatus(kDisconnected);
        _channel->disableAll(); 
        _connectionCallback(shared_from_this());
    }
    _channel->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
    if (n > 0)
    {
        _messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else 
    {
        errno = savedErrno;
        Logger::getInstance().logERROR("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (_channel->isWriting())
    {
        int savedErrno = 0;
        ssize_t n = _outputBuffer.writeFd(_channel->fd(), &savedErrno);
        if (n > 0)
        {
            _outputBuffer.retrieve(n);
            if (_outputBuffer.readableBytes() == 0)
            {
                _channel->disableWriting();
                if (_writeCompleteCallback)
                {
                    _baseloop->queueInLoop(
                        std::bind(_writeCompleteCallback, shared_from_this()));
                }
                if (_status == kDisconnecting)
                {
                    shutDownInLoop(); 
                }
            }
        }
        else
        {
            Logger::getInstance().logERROR("TcpConnection::handleWrite");
        }
    }
    else
    {
        Logger::getInstance().logERROR("TcpConnection fd=" + to_string(_channel->fd()) + "is down, no more writing");
    }
}

void TcpConnection::handleClose()
{
    Logger::getInstance().logINFO("TcpConnection::handleClose fd=" + to_string(_channel->fd()) + " state=" + to_string((int)_status));
    setStatus(kDisconnected);
    _channel->disableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    _connectionCallback(connPtr);
    _closeCallback(connPtr);     
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t optlen = sizeof optval;
    int err = 0;
    if (::getsockopt(_channel->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    Logger::getInstance().logERROR("TcpConnection::handleError name:" + _name + " - SO_ERROR:" + to_string(err));
}