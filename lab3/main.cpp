// run g++ -fopenmp -o file main.cpp or g++ -openmp -o file main.cpp
#include <iostream>
#include "matrix_dot_vector.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    test<int>(1 << 10, 1 << 20);
    return 0;
}
