#ifndef LAB2_FIXED_ATOMIC_QUEUE_H
#define LAB2_FIXED_ATOMIC_QUEUE_H

#include "interface_queue.h"
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <condition_variable>

template <typename T>
class FixedAtomicQueue: public IQueue<T> {
private:
    std::vector<T> arr;
    std::atomic_size_t head, tail, size;
    std::mutex mutex_pop, mutex_push;
    std::condition_variable cond_var_pop, cond_var_push;
    std::atomic_bool free;

public:
    FixedAtomicQueue(size_t s): free(true), head(0), tail(0), size(0) {
        arr.resize(s);
    }
    bool pop(T &v);
    void push(T v);
};

template <typename T>
void FixedAtomicQueue<T>::push(T v) {
    for (;;) {
        bool required = true;
        bool is_free = free.compare_exchange_strong(required, false);
        while (!is_free) {
            std::unique_lock<std::mutex> lock(mutex_push);
            cond_var_push.wait(lock, [&]() {
                bool b = true;
                return free.compare_exchange_strong(b, false);
            });
            is_free = true;
        }
        if (size < arr.size())
            break;
        else {
            free = true;
            cond_var_pop.notify_one();
        }
    }
    arr[tail] = v;
    tail = (tail + 1) % arr.size();
    ++size;
    free = true;
    cond_var_pop.notify_one();
}

template <typename T>
bool FixedAtomicQueue<T>::pop(T &v) {
    bool result = false;
    bool required = true;
    bool is_free = free.compare_exchange_strong(required, false);
    if (!is_free) {
        std::unique_lock<std::mutex> lock(mutex_pop);
        is_free = cond_var_pop.wait_for(lock, std::chrono::milliseconds(1), [&](){
            bool required = true;
            return free.compare_exchange_strong(required, false);
        });
    }
    if (is_free) {
        if (size > 0) {
            v = arr[head];
            head = (head + 1) % arr.size();
            size--;
            result = true;
        }
        free = true;
        cond_var_push.notify_one();
    }
    return result;
}

#endif //LAB2_FIXED_ATOMIC_QUEUE_H
