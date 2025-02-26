#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int min, int max) : _maxThreads(max),
                                           _minThreads(min), _stop(false), _exitNumber(0)
{
    _idleThreads = _curThreads = min;
    _manager = new thread(&ThreadPool::manager, this);
    for (int i = 0; i < _curThreads; ++i)
    {
        thread t(&ThreadPool::worker, this);
        _workers.insert(make_pair(t.get_id(), move(t)));
    }
}

ThreadPool::~ThreadPool()
{
    _stop = true;
    _condition.notify_all();
    for (auto &it : _workers)
    {
        thread &t = it.second;
        if (t.joinable())
        {
            t.join();
        }
    }
    if (_manager->joinable())
    {
        _manager->join();
    }
    delete _manager;
}

// void ThreadPool::addTask(function<void()> f)
// {
//     {
//         lock_guard<mutex> locker(_queueMutex);
//         _tasks.emplace(f);
//     }
//     _condition.notify_one();
// }

void ThreadPool::manager()
{
    while (!_stop.load())
    {
        this_thread::sleep_for(chrono::seconds(2));
        int idle = _idleThreads.load();
        int current = _curThreads.load();
        if (idle > current / 2 && current > _minThreads)
        {
            _exitNumber.store(2);
            _condition.notify_all();
            unique_lock<mutex> lck(_idsMutex);
            for (const auto &id : _ids)
            {
                auto it = _workers.find(id);
                if (it != _workers.end())
                {
                    (*it).second.join();
                    _workers.erase(it);
                }
            }
            _ids.clear();
        }
        else if (idle == 0 && current < _maxThreads)
        {
            thread t(&ThreadPool::worker, this);
            _workers.insert(make_pair(t.get_id(), move(t)));
            _curThreads++;
            _idleThreads++;
        }
    }
}

void ThreadPool::worker()
{
    while (!_stop.load())
    {
        function<void()> task = nullptr;
        {
            unique_lock<mutex> locker(_queueMutex);
            while (!_stop && _tasks.empty())
            {
                _condition.wait(locker);
                if (_exitNumber.load() > 0)
                {
                    _exitNumber--;
                    _curThreads--;
                    unique_lock<mutex> lck(_idsMutex);
                    _ids.emplace_back(this_thread::get_id());
                    return;
                }
            }

            if (!_tasks.empty())
            {
                task = move(_tasks.front());
                _tasks.pop();
            }
        }

        if (task)
        {
            _idleThreads--;
            task();
            _idleThreads++;
        }
    }
}