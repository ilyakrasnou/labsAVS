//
// Created by ilya on 4.11.19.
//

#ifndef LAB3_MATRIX_DOT_VECTOR_H
#define LAB3_MATRIX_DOT_VECTOR_H

#include <vector>
#include <omp.h>
#include <chrono>
#include <iostream>


template <typename T>
std::vector<T> dot(const std::vector<std::vector<T>>& matrix,
                            const std::vector<T>& vector) {
    std::vector<T> result(matrix.size(), 0);
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < vector.size(); ++j)
            result[i] += matrix[i][j] * vector[j];
    }
    return result;
}


template <typename T>
std::vector<T> parallel_dot(const std::vector<std::vector<T>>& matrix,
                              const std::vector<T>& vector) {
    std::vector<T> result(matrix.size(), 0);
#pragma omp parallel for collapse(2)
    for (int i = 0; i < matrix.size(); ++i) {
// эффективно ли с reduction?
#pragma for reduction (+, sum)
         for (int j = 0; j < vector.size(); ++j)
            result[i] += matrix[i][j] * vector[j];
    }
    return result;
}


template <typename T>
void test(int n, int m) {
    std::vector<std::vector<T>> matrix(n, std::vector<T>(m, 512));
    std::vector<T> vector(m, 23);
    auto start = std::chrono::high_resolution_clock::now();
    auto res = parallel_dot<T>(matrix, vector);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::endl << "Time for parallel computation: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
              << std::endl;
    start = std::chrono::high_resolution_clock::now();
    res = dot<T>(matrix, vector);
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::endl << "Time for non-parallel computation: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1e-9 << " sec"
              << std::endl;
//    for (auto i: res) {
//        std::cout << i << ' ';
//    }
//    std::cout << std::endl;
}

#endif //LAB3_MATRIX_DOT_VECTOR_H
