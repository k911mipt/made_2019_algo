#pragma once
#ifndef COMMON_SORT_H_
#define COMMON_SORT_H_

#include <random>
#include <cassert>
#include <iostream>
#include "sort.h"

namespace made {
    typedef uint8_t Digit;
    //typedef uint16_t Digit;
    typedef uint32_t Element; // 0..10^9 value range
    typedef uint32_t ElementCounter; // 0..10^7 array size
    const uint8_t DIGIT_SIZE = sizeof(Digit);
    const uint32_t MAX_DIGIT_COUNT = 1 << (DIGIT_SIZE * 8);
    // TODO which type to use for mask? 32 or 8 bit? Check how different mask types applies
    const Element LOWER_DIGIT_MASK = MAX_DIGIT_COUNT - 1; // digit = 1 byte: 0xFF, digit = 2 byte: 0xFFFF, etc.;
}

using made::Element;
using made::ElementCounter;

typedef void(*sorting_size)(Element* arr, ElementCounter n);
typedef void(*sorting_range)(Element* begin, Element* end);

namespace common {


    const ElementCounter MAX_VALUE = 1000000000;
    ElementCounter test_size = 10000;
    //const size_type MAX_VALUE = 1000;// 0000000;

    inline void generate(Element *arr, ElementCounter n, bool silent = false, ElementCounter range = MAX_VALUE) {
        if (!silent) std::cout << "generating " << n << " randoms in range " << 0 << "-" << range << std::endl;
        std::default_random_engine generator;
        std::uniform_int_distribution<ElementCounter> distribution(0, range);
        for (Element* end = arr + n; arr < end; ++arr) {
            *arr = distribution(generator);
        }
    }

    void test(sorting_size func, bool silent = false) {
        std::cout << "testing... ";
        Element* arr = new Element[test_size];
        Element* arr_correct = new Element[test_size];
        generate(arr, test_size, silent, test_size * 40);
        std::copy(arr, arr + test_size, arr_correct);
        std::sort(arr_correct, arr_correct + test_size);
        func(arr, test_size);
        for (ElementCounter i = 0; i < test_size; ++i)
            assert(arr[i] == arr_correct[i]);
        delete[] arr;
        delete[] arr_correct;
    }

    void test(sorting_range func, bool silent = false) {
        std::cout << "testing... ";
        Element* arr = new Element[test_size];
        Element* arr_correct = new Element[test_size];
        generate(arr, test_size, silent, test_size * 40);
        std::copy(arr, arr + test_size, arr_correct);
        std::sort(arr_correct, arr_correct + test_size);
        func(arr, arr + test_size);
        for (ElementCounter i = 0; i < test_size; ++i)
            assert(arr[i] == arr_correct[i]);
        delete[] arr;
        delete[] arr_correct;
    }

    inline void input(Element *arr, ElementCounter n) {
        for (ElementCounter i = 0; i < n; ++i, ++arr) {
            std::cin >> *arr;
        }
    }

    inline void output(Element *arr, ElementCounter n) {
        for (ElementCounter i = 0; i < n; ++i, ++arr) {
            std::cout << *arr << " ";
        }
        std::cout << std::endl;
    }

    inline void output10(Element *arr, ElementCounter n) {
        for (ElementCounter i = 0; i < 10; ++i) {
            for (ElementCounter j = 0; j < n / 10; ++j) {
                std::cout << arr[j * 10 + i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

#endif // !COMMON_SORT_H_

