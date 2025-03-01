#include "CurrentThread.h"

namespace CurrentThread {

    __thread int _cachedTid = 0;

    void cachedTid() {
        if(_cachedTid == 0)
            _cachedTid = ::syscall(SYS_gettid);
    }
}