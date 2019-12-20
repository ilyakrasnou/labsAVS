#ifndef LAB2_ARRAY_TRAVERSAL_H
#define LAB2_ARRAY_TRAVERSAL_H

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

template <typename T>
bool inc_mutex(std::vector<T>& arr, size_t& index, std::mutex& mutex, int sleep) {
    size_t old_index;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (index >= arr.size())
            return false;
        old_index = index;
        index++;
    }
    if (sleep) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
    }
    arr[old_index]++;
    return true;
}

template<typename T>
bool inc_atomic(std::vector<T>& arr, std::atomic_size_t& index, int sleep) {
    size_t old_index = index.fetch_add(1);
    if (old_index >= arr.size())
        return false;
    if (sleep) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
    }
    arr[old_index]++;
    return true;
}

template <typename T>
void execute_mutex(size_t num_el, int num_threads, int sleep = 0) {
    std::vector<T> arr(num_el, 0);
    std::mutex mutex;
    size_t index = 0;
    std::vector<std::thread> threads(num_threads);

    for (auto& thread: threads) {
        thread = std::thread([sleep](std::vector<T>& arr, size_t& i, std::mutex& m) {
            while(inc_mutex(arr, i, m, sleep));
        }, std::ref(arr), std::ref(index), std::ref(mutex));
    }
    for (auto& thread: threads)
        if (thread.joinable())
            thread.join();
    for (T i: arr)
        if (i != 1)
            std::cout << "Ooops!" << std::endl;
}

template <typename T>
void execute_atomic(size_t num_task, int num_thread, int sleep = 0) {
    std::vector<T> arr(num_task, 0);
    std::atomic_size_t index(0);
    std::vector<std::thread> threads(num_thread);
    for (auto& thread: threads) {
        thread = std::thread([sleep](std::vector<T>& arr, std::atomic_size_t& index) {
            while (inc_atomic(arr, index, sleep));
        }, std::ref(arr), std::ref(index));
    }
    for (auto& thread: threads) {
        if (thread.joinable())
            thread.join();
    }
    for (T i: arr)
        if (i != 1)
            std::cout << "Ooops!" << std::endl;
}

template <typename T>
void run_array_traversal() {
    std::vector<int>  num_threads = {4, 8, 16, 32};
    std::vector<int> sleeps = {0, 10};
    size_t num_task = 1 << 20;
    for (auto sleep: sleeps)
        for (auto num_thread: num_threads) {
            auto start = std::chrono::high_resolution_clock::now();
            execute_mutex<T>(num_task, num_thread, sleep);
            auto end = std::chrono::high_resolution_clock::now();
            auto mutex_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() * 1e-9;
            start = std::chrono::high_resolution_clock::now();
            execute_atomic<T>(num_task, num_thread, sleep);
            end = std::chrono::high_resolution_clock::now();
            auto atomic_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() * 1e-9;
            std::cout << "NumThreads: " << num_thread << ", sleep: " << sleep << std::endl
                      << "mutex: " << mutex_time << " sec" << std::endl
                      << "atomic: " << atomic_time << " sec" << std::endl << std::endl;
        }
}

#endif //LAB2_ARRAY_TRAVERSAL_H
