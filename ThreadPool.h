#pragma once

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <map>
#include <memory>
#include <future>
using namespace std;

// 线程池类
class ThreadPool
{
public:
    ThreadPool(int min = 4, int max = thread::hardware_concurrency()*128);
    ~ThreadPool();
    //void addTask(function<void()> f);
    template<typename F, typename... Args>
    auto addTask(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type>
    {
        using return_type = typename result_of<F(Args...)>::type;
        auto task = make_shared<packaged_task<return_type()>> (
            bind(forward<F>(f), forward<Args>(args)...)
            );
        future<return_type> res = task->get_future();

        _queueMutex.lock();
        _tasks.emplace([task]() { 
            (*task)(); 
            });
        _queueMutex.unlock();
        _condition.notify_one();

        return res;
    }

private:
    void manager();
    void worker();

private:
    thread *_manager;
    map<thread::id, thread> _workers;
    vector<thread::id> _ids;
    int _minThreads;
    int _maxThreads;
    atomic<bool> _stop;
    atomic<int> _curThreads;
    atomic<int> _idleThreads;
    atomic<int> _exitNumber;
    queue<function<void()>> _tasks;
    mutex _idsMutex;
    mutex _queueMutex;
    condition_variable _condition;
};

#endif