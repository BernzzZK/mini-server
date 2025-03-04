#include "Buffer.h"
#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int *saveError) {
    char buff[65535] = {0};
    struct iovec vec[2];
    const size_t writeable = writeableBytes();

    vec[0].iov_base = begin() + _writeIdx;
    vec[0].iov_len = writeable;
    vec[1].iov_base = buff;
    vec[1].iov_len = sizeof(buff);

    const int iovcnt = (writeable < sizeof(buff)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if(n < 0) {
        *saveError = errno;
    } else if (n < writeable) {
        _writeIdx += n;
    } else {
        _writeIdx = _buffer.size();
        append(buff, n - writeable);
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int *saveError) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if(n < 0)
        *saveError = errno;
    return n;
}