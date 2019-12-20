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


/*struct QueueConfig {
    int retries = 2;
};

class IQueue {
public:
    IQueue(const QueueConfig& config)
            : config(config)
    {}

    // Записывает элемент в очередь.
    // Гсли очередь фиксированного размер и заполнена,
    // поток повисает внутри функции пока не освободится место
    virtual void push(uint8_t val) = 0;
    // Если очередь пуста, ждем 1 мс записи в очередь.
    // Если очередь не пуста, помещает значение головы в val,
    // удаляет голову и возвращает true.
    // Если очередь по прежнему пуста, возвращаем false
    virtual bool pop(uint8_t& val) = 0;

    void one_producer_started() {
        thread_count.fetch_add(1);
    }

    void one_producer_finished() {
        thread_count.fetch_sub(1);
    }

    bool is_producing_finished() {
        return thread_count.load() == 0;
    }
protected:
    QueueConfig config;
private:
    std::atomic<int> thread_count;
};*/

//template<typename V>
//struct tagged_ptr {
//    V* ptr;
//    int ref;
//
//    tagged_ptr()
//            : ptr(nullptr)
//            , ref(0)
//    {}
//
//    tagged_ptr(V* p)
//            : ptr(p)
//            , ref(0)
//    {}
//
//    tagged_ptr(V * p, unsigned int n)
//            : ptr(p)
//            , ref(n)
//    {}
//
//    V* operator->() const {
//        return ptr;
//    }
//
//    bool operator == (const tagged_ptr& t) const {
//        return ptr == t.ptr && ref == t.ref;
//    }
//
//    bool operator != (const tagged_ptr& t) const {
//        return ptr != t.ptr || ref != t.ref;
//    }
//};
//
//template <typename K>
//struct node {
//    K data;
//    std::atomic<tagged_ptr<node<K>>> next;
//
//    node()
//            : next()
//            , data(K())
//    {}
//};
//
//
//template <typename T>
//class DynamicNoLockQueue : public IQueue<T> {
//private:
//    node<T>* dummy = new node<T>();
//    std::atomic<tagged_ptr<node<T>>> head = new tagged_ptr<node<T>>(dummy, 0);
//    std::atomic<tagged_ptr<node<T>>> tail = new tagged_ptr<node<T>>(dummy, 0);
//public:
//    DynamicNoLockQueue()
//    {
////        auto dummy = new node<T>();
////        head.store(tagged_ptr<node<T>>(dummy, 0));
////        tail.store(tagged_ptr<node<T>>(dummy, 0));
//        std::cout << head.is_lock_free();
//        if (std::atomic_is_lock_free(&head)) {
//            std::cout << "Hooray!" << std::endl;
//        } else {
//            std::cout << "Fuck!" << std::endl;
//        }
//    }
//
//    void push(T val) override {
//        node<T>* newNode = new node<T>();
//        newNode->data = val;
//        newNode->next.store(tagged_ptr<node<T>>());
//
//        while (true) {
//            tagged_ptr<node<T>> last = tail.load();
//            tagged_ptr<node<T>> next = last->next;
//
//            if (last != tail.load()) continue;
//            if (next.ptr == nullptr) {
//                // мы в конце очереди
//                if (tail.load()->next.compare_exchange_weak(next, tagged_ptr<node<T>>(newNode, next.ref + 1))) {
//                    tail.compare_exchange_strong(last, tagged_ptr<node<T>>(newNode, last.ref + 1));
//                    break;
//                }
//            } else {
//                //мы не в конце, поэтому продвигаем тейл дальше
//                tail.compare_exchange_strong(last, tagged_ptr<node<T>>(next.ptr, last.ref + 1));
//            }
//        }
//    }
//
//    bool pop(T& val) override {
//        for (int retries = 0; retries < 2; retries++) {
//
//            while (true) {
//                tagged_ptr<node<T>> first = head.load();
//                tagged_ptr<node<T>> last = tail.load();
//                tagged_ptr<node<T>> next = first->next.load();
//
//                if (first == head.load()) {
//                    // пустая очередь
//                    if (first.ptr == last.ptr) {
//                        if (next.ptr == nullptr) {
//                            // пустая очередь
//                            break;
//                        }
//                        // tail.next != null, продвигаем
//                        tail.compare_exchange_strong(last, tagged_ptr<node<T>>(next.ptr, last.ref + 1));
//                    } else { //
//                        val = next->data;
//                        // первый двигаем на next
//                        if (head.compare_exchange_weak(first, tagged_ptr<node<T>>(next.ptr, first.ref + 1))) {
//                            // controlled_mem_leak(head); //проблема чтения из мусорной памяти
//                            return true;
//                        }
//                    }
//                }
//            }
//
//            if (retries != 2- 1) {
//                std::this_thread::sleep_for(std::chrono::nanoseconds(500));
//            }
//        }
//        return false;
//    }
//};

#endif //LAB2_LOCK_FREE_QUEUE_H
