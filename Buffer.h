#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include <algorithm>
#include <string>
#include <stddef.h>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    explicit Buffer(size_t initialSize = kInitialSize)
        : _readIdx(kCheapPrepend)
        , _writeIdx(kCheapPrepend)
        , _buffer(kCheapPrepend + initialSize)
        { }

        size_t readableBytes() { return _writeIdx - _readIdx; }
        size_t writeableBytes() { return _buffer.size() - _writeIdx; }
        size_t prependableBytes() { return _readIdx; }

        const char *peek() const { return begin() + _readIdx; }

        void retrieve(size_t len) {
            if(len < readableBytes())
                _readIdx += len;
            else
                retrieveAll();
        }
        void retrieveAll() {
            _readIdx = kCheapPrepend;
            _writeIdx = kCheapPrepend;
        }

        std::string retrieveAllAsString() {
            return retrieveAsString(readableBytes());
        }
        std::string retrieveAsString(size_t len) {
            std::string res(peek(), len);
            retrieve(len);
            return res;
        }

        void ensureWriteBytes(size_t len) {
            if(writeableBytes() < 0) {
                makeSpace(len);
            }
        }

        void append(const char* data, size_t len) {
            ensureWriteBytes(len);
            std::copy(data, data + len, beginWrite());
            _writeIdx += len;
        }
        char *beginWrite() { return begin() + _writeIdx; }

        ssize_t readFd(int fd, int *saveError);
        ssize_t writeFd(int fd, int *saveError);

    private:
        char *begin() { return &*_buffer.begin(); }
        const char *begin() const { return &*_buffer.begin(); }
        void makeSpace(size_t len) {
            if(writeableBytes() + prependableBytes() < len + kCheapPrepend){
                _buffer.resize(_writeIdx + len);
            } else {
                size_t readable = readableBytes();
                std::copy(begin() + _readIdx,
                          begin() + _writeIdx,
                          begin() + kCheapPrepend);
                _readIdx = kCheapPrepend;
                _writeIdx = kCheapPrepend + readable;
            }
        }
        size_t _readIdx;
        size_t _writeIdx;
        std::vector<char> _buffer;
};

#endif