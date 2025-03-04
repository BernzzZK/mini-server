#include "InetAddress.h"
#include <strings.h>
#include <string.h>


InetAddress::InetAddress(uint16_t port, std::string ip) {
    bzero(&_addr, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAddress::toIP() const {
    char buff[64] = {0};
    ::inet_ntop(AF_INET, &_addr.sin_addr.s_addr, buff, sizeof(buff));
    return buff;
}

std::string InetAddress::toIPPort() const {
    char buff[64] = {0};
    ::inet_ntop(AF_INET, &_addr.sin_addr.s_addr, buff, sizeof(buff));
    size_t end = ::strlen(buff);
    uint16_t port = ::ntohs(_addr.sin_port);
    sprintf(buff + end, ":%u", port);
    return buff;
}

uint16_t InetAddress::toPort() const {
    uint16_t port;
    port = ::ntohs(_addr.sin_port);
    return port;
}


//test
#if 0
#include <iostream>
using namespace std;
int main()
{
    InetAddress addr;
    cout << addr.toIP() << endl;
    cout << addr.toIPPort() << endl;
    cout << addr.toPort() << endl;
    return 0;
}
#endif