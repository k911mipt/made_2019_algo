#pragma once
#ifndef COMMON_SORT_H_
#define COMMON_SORT_H_

#include <random>
#include "sort.h"


typedef uint32_t size_type;

namespace common {
    const size_type MAX_VALUE = 1000000000;

    inline void input(size_type *arr, size_type n) {
        for (size_type i = 0; i < n; ++i, ++arr) {
            std::cin >> *arr;
        }
    }

    inline void generate(size_type *arr, size_type n) {
        std::default_random_engine generator;
        std::uniform_int_distribution<size_type> distribution(0, MAX_VALUE);
        for (size_type i = 0; i < n; ++i, ++arr) {
            *arr = distribution(generator);
        }
    }

    inline void output(size_type *arr, size_type n) {
        for (size_type i = 0; i < n; ++i, ++arr) {
            std::cout << *arr << " ";
        }
        std::cout << std::endl;
    }
    inline void output10(size_type *arr, size_type n) {
        for (size_type i = 0; i < 10; ++i) {
            for (size_type j = 0; j < n / 10; ++j) {
                std::cout << arr[j * 10 + i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

}

#endif // !COMMON_SORT_H_

