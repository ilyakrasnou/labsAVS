//
// Created by ilya on 7.11.19.
//
// source https://neerc.ifmo.ru/wiki/index.php?title=%D0%9E%D1%87%D0%B5%D1%80%D0%B5%D0%B4%D1%8C_%D0%9C%D0%B0%D0%B9%D0%BA%D0%BB%D0%B0_%D0%B8_%D0%A1%D0%BA%D0%BE%D1%82%D1%82%D0%B0

#ifndef LAB2_LOCK_FREE_QUEUE_H
#define LAB2_LOCK_FREE_QUEUE_H

#include <atomic>
#include "interface_queue.h"


template <typename T>
class Node {
public:
    T value;
    std::shared_ptr<Node<T>> next;
    Node(T val): next(nullptr) {
        value = val;
    }
};


template <typename T>
class LockFreeQueue: public IQueue<T> {
    alignas(128) std::shared_ptr<Node<T>> head;
    alignas(128) std::shared_ptr<Node<T>> tail;
public:
    LockFreeQueue (): head(new Node<T>(T())) {
        tail = std::atomic_load(&head);
    }
    void push(T v);
    bool pop(T& v);
};


template <typename T>
void LockFreeQueue<T>::push(T v) {
    std::shared_ptr<Node<T>> node(new Node<T>(v));
    for (;;) {
        std::shared_ptr<Node<T>> last = std::atomic_load(&tail);
        std::shared_ptr<Node<T>> null_pointer(nullptr);
        if (std::atomic_compare_exchange_strong(&(last->next), &null_pointer, node)) {
            std::atomic_compare_exchange_strong(&tail, &last, node);
            return;
        } else {
            std::atomic_compare_exchange_strong(&tail, &last, last->next);
        }
    }
}


template <typename T>
bool LockFreeQueue<T>::pop(T &v) {
    T result;
    for (;;) {
        std::shared_ptr<Node<T>> first = std::atomic_load(&head);
        std::shared_ptr<Node<T>> last = std::atomic_load(&tail);
        std::shared_ptr<Node<T>> next = std::atomic_load(&(first->next));
        if (first == last) {
            if (next == nullptr) {
                return false;
            }
        } else {
            result = next->value;
            if (std::atomic_compare_exchange_strong(&head, &first, next)) {
                v = result;
                return true;
            }
        }
    }
}

#endif //LAB2_LOCK_FREE_QUEUE_H
