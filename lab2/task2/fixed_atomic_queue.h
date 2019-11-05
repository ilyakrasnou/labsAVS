#ifndef LAB2_FIXED_ATOMIC_QUEUE_H
#define LAB2_FIXED_ATOMIC_QUEUE_H

#include "interface_queue.h"
#include <thread>
#include <vector>
#include <atomic>

template <typename T>
class FixedAtomicQueue: public IQueue<T> {
private:
    std::vector<T> arr;
    size_t capacity;
    alignas(128) std::atomic_size_t head;
    alignas(128) std::atomic_size_t tail;
    std::atomic_bool is_writing;

public:
    FixedAtomicQueue(size_t s): head(0), tail(0), is_writing(false) {
        arr.resize(s);
        capacity = s;
    }
    bool pop(T &v);
    void push(T v);
};

template <typename T>
void FixedAtomicQueue<T>::push(T v) {
    for (;;) {
        size_t tail_pos = tail;
        // Проверка на переполнение
        if ((tail_pos + 1) % capacity == head)
            continue;
        // очередь пуста, просто ждем
        T tail_value = arr[tail_pos];
        if (tail_pos != tail)
            break;
        // Ну не знаю я, как по-другому реализовать DCAS
        bool smb_write = false;
        if (is_writing.compare_exchange_strong(smb_write, true)) {
            if (arr[tail_pos] == tail_value && tail == tail_pos) {
                arr[tail_pos] = v;
                tail = (tail_pos + 1) % capacity;
                is_writing = false;
                return;
            }
            is_writing = false;
        }

    }
}

template <typename T>
bool FixedAtomicQueue<T>::pop(T &v) {
    for(;;) {
        size_t head_pos = head;
        // очередь пуста, подождем и выйдем
        if (head_pos == tail) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            head_pos = head;
            if (head_pos == tail)
                return false;
        }
        T head_value = arr[head_pos];
        if (head.compare_exchange_strong(head_pos, (head_pos+1) % capacity)) {
            v = head_value;
            return true;
        }
    }
}

#endif //LAB2_FIXED_ATOMIC_QUEUE_H
