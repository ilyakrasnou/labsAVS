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
    size_t capacity;
    std::vector<T> arr;
    std::mutex mutex_queue;
    std::condition_variable cond_var;
    alignas(128) size_t head;
    alignas(128) size_t tail;
    alignas(128) size_t size;

public:
    FixedMutexQueue(size_t s) : head(0), tail(0), size(0) {
        arr.resize(s);
        capacity = s;
    }
    void push(T v);
    bool pop(T &v);
};

template <typename T>
void FixedMutexQueue<T>::push(T v) {
    std::unique_lock<std::mutex> lock(mutex_queue);
    cond_var.wait(lock, [&] { return size < capacity; });
    arr[tail] = v;
    tail = (tail + 1) % capacity;
    ++size;
    cond_var.notify_one();
}

template <typename T>
bool FixedMutexQueue<T>::pop(T &v) {
    std::unique_lock<std::mutex> lock(mutex_queue);
    cond_var.notify_all();
    if (cond_var.wait_for(lock, std::chrono::milliseconds(1), [&]{return size > 0;})) {
        v = arr[head];
        head = (head + 1) % capacity;
        --size;
        cond_var.notify_one();
        return true;
    }
    else
        return false;
}
#endif //LAB2_FIXED_MUTEX_QUEUE_H
