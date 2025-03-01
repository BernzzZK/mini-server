#include "Thread.h"
#include <semaphore.h>
#include "CurrentThread.h"

std::atomic_int _numCreated(0);

Thread::Thread(ThreadFunc threadFunc, const std::string& name)
    : _start(false)
    , _join(false)
    , _tid(0)
    , _threadFunc(std::move(threadFunc))
    , _name(name)
{
    setDefaultName();
}

Thread::~Thread() {
    if(!_join && _start)
        _thread->detach();
}

void Thread::setDefaultName() {
    int num = ++_numCreated;
    if(_name.empty()) {
        char buff[32] = {0};
        snprintf(buff, sizeof(buff), "Thread%d", num);
        _name = buff;
    }
}

void Thread::join() {
    _join = true;
    _thread->join();
}

void Thread::start() {
    _start = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    _thread = std::unique_ptr<std::thread>(new std::thread([&]() { 
        _tid = CurrentThread::tid();
        sem_post(&sem);
        _threadFunc();
    }));
    sem_wait(&sem);
}