#ifndef LAB2_DYNAMIC_QUEUE_H
#define LAB2_DYNAMIC_QUEUE_H

#include "interface_queue.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>


template <typename T>
class DynamicQueue: public IQueue<T> {
private:
    std::queue<T> queue;
    std::mutex mutex;

public:
    DynamicQueue() {}
    void push(T val);
    bool pop(T &val);
};

template <typename T>
void DynamicQueue<T>::push(T val) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(val);
}

template <typename T>
bool DynamicQueue<T>::pop(T &val) {
    std::lock_guard<std::mutex> lock(mutex);
    bool result = false;
    if (!queue.empty()) {
        val = queue.front();
        queue.pop();
        result = true;
    }
    return result;
}

#endif //LAB2_DYNAMIC_QUEUE_H
