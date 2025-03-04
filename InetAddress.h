#pragma once
#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, std::string IP = "127.0.0.1");
    explicit InetAddress(sockaddr_in addr)
        : _addr(addr){}

    std::string toIP() const;
    std::string toIPPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getAddr() const { return &_addr; }
    void setAddr(const sockaddr_in &addr) { _addr = addr; }

private:
    sockaddr_in _addr;
};

#endif