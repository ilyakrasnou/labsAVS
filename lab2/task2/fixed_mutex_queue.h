#ifndef LAB2_FIXED_MUTEX_QUEUE_H
#define LAB2_FIXED_MUTEX_QUEUE_H

#include "interface_queue.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>


template <typename T>
class FixedMutexQueue: public IQueue<T> {
private:
    std::vector<T> arr;
    std::mutex mutex_pop, mutex_push, mutex_queue;
    std::condition_variable cond_var_push, cond_var_pop;
    size_t head, tail, size;

public:
    FixedMutexQueue(size_t s) : head(0), tail(0), size(0) {
        arr.resize(s);
    }
    void push(T v);
    bool pop(T &v);
};

template <typename T>
void FixedMutexQueue<T>::push(T v) {
    std::unique_lock<std::mutex> lock(mutex_push);
    cond_var_push.wait(lock, [&] { return size < arr.size(); });
    arr[tail] = v;
    tail = (tail + 1) % arr.size();
    {
        std::lock_guard<std::mutex> lock_q(mutex_queue);
        ++size;
    }
    cond_var_pop.notify_one();
}

template <typename T>
bool FixedMutexQueue<T>::pop(T &v) {
    std::unique_lock<std::mutex> lock(mutex_pop);
    cond_var_push.notify_all();
    if (cond_var_pop.wait_for(lock, std::chrono::milliseconds(1), [&]{return size > 0;})) {
        v = arr[head];
        head = (head + 1) % arr.size();
        {
            std::lock_guard<std::mutex> lock_q(mutex_queue);
            --size;
        }
        cond_var_push.notify_one();
        return true;
    }
    else
        return false;
}
#endif //LAB2_FIXED_MUTEX_QUEUE_H
