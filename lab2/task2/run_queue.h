#ifndef LAB2_RUN_QUEUE_H
#define LAB2_RUN_QUEUE_H

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "dynamic_queue.h"
#include "fixed_atomic_queue.h"
#include "fixed_mutex_queue.h"
#include "lock_free_queue.h"

const std::vector<int> producer_num = {1, 2, 4};
const std::vector<int> consumer_num = {1, 2, 4};
const std::vector<size_t> queue_size = {1, 4, 16};
const int NUM_TASK = 1 << 20;

template <typename T>
void test_queue(IQueue<T> &queue, int producer_n, int consumer_n) {
    int task_num = NUM_TASK;
    std::atomic_int sum(0);
    std::vector<std::thread> producers(producer_n);
    std::vector<std::thread> consumers(consumer_n);
    auto start = std::chrono::high_resolution_clock::now();
    for (auto& thread: producers)
        thread = std::thread([&task_num, &queue]() {
            for (int i = 0; i < task_num; ++i)
                queue.push(1);
        });
    for (auto& thread: consumers)
        thread = std::thread([&sum, &queue, task_num, producer_n]() {
            T v;
            while (sum.load() < producer_n * task_num) {
                sum += queue.pop(v);
            }
        });
    for (auto& thread: producers) {
        if (thread.joinable())
            thread.join();
    }
    for (auto& thread: consumers) {
        if (thread.joinable())
            thread.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout <<" " << sum.load() << ", time: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
              << std::endl << std::endl;
}

template <typename T>
void run_dynamic_queue() {
    for (int producer_n : producer_num)
        for (int consumer_n : consumer_num) {
            std::cout << "Result for producer_num: " << producer_n
                      << ", consumer_num: " << consumer_n << std::endl;
            DynamicQueue<T> q;
            test_queue<T>( q, producer_n, consumer_n);
        }
}

template <typename T>
void run_fixed_mutex_queue() {
    for (auto size : queue_size)
        for (int producer_n : producer_num)
            for (int consumer_n : consumer_num) {
                std::cout << "Result for producer_num: " << producer_n
                          << ", consumer_num: " << consumer_n
                          << ", size: " << size << std::endl;
                FixedMutexQueue<T> q(size);
                test_queue<T>(q, producer_n, consumer_n);
            }
}

template <typename T>
void run_fixed_atomic_queue() {
    for (auto size : queue_size)
        for (int producer_n : producer_num)
            for (int consumer_n : consumer_num) {
                std::cout << "Result for producer_num: " << producer_n
                          << ", consumer_num: " << consumer_n
                          << ", size: " << size << std::endl;
                FixedMutexQueue<T> q(size);
                test_queue<T>(q, producer_n, consumer_n);
            }
}

template <typename T>
void run_lock_free_queue() {
    for (auto size : queue_size)
        for (int producer_n : producer_num)
            for (int consumer_n : consumer_num) {
                std::cout << "Result for producer_num: " << producer_n
                          << ", consumer_num: " << consumer_n
                          << ", size: " << size << std::endl;
                LockFreeQueue<T> q;
                test_queue<T>(q, producer_n, consumer_n);
            }
}


#endif //LAB2_RUN_QUEUE_H
