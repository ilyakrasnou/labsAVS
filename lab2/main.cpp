#include <iostream>
#include "task1/array_traversal.h"
#include "task2/run_queue.h"

// compile g++ -pthread -o programm main.cpp

int main() {
    char answer;
    bool exit = false;
    while (!exit) {
        std::cout << "1 - 1 task, 2 - 2.1, 3 - 2.2, 4 - 2.3, 5 - 2.4, other - to exit ";
        std::cin >> answer;
        switch (answer) {
            case '1':
                run_array_traversal<uint8_t>();
                break;
            case '2':
                run_dynamic_queue<uint8_t>();
                break;
            case '3':
                run_fixed_mutex_queue<uint8_t>();
                break;
            case '4':
                run_fixed_atomic_queue<uint8_t>();
                break;
            case '5':
                run_lock_free_queue<uint8_t>();
                break;
            default:
                exit = true;
        }
    }
    return 0;
}