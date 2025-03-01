#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread {
    extern __thread int _cachedTid;
    void cachedTid();

    inline int tid() {
        if(__builtin_expect(_cachedTid == 0, 0)) {
            cachedTid();
        }
        return _cachedTid;
    }
}