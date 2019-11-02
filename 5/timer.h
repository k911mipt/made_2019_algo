
#ifndef TIMER_H_
#define TIMER_H_
#include <chrono>
#include <iostream>
#include <string>

#include "common.h"

int tests_count = 1;
Element algo_number = 1;
bool show_name = true;


typedef long long ms;

class Timer
{
    using clock_t = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
public:
    Timer() { start_ = clock_t::now(); }
    ~Timer() = default;
    ms Finish() {
        const auto finish = clock_t::now();
        return std::chrono::duration_cast<microseconds>(finish - start_).count();
    }
private:
    clock_t::time_point start_;
};
using std::string;

ms TimeIt(Element* arr, ElementCounter n, sorting_size func, const string&& name) {
    common::test(func, true);
    std::cout << "sorting algo " << algo_number++ << ": ";
    if (show_name) {
        string display_name(name); display_name.resize(20);
        std::cout << display_name << " \t";
    }
    ms result = 0;
    Element* arr_copy = new Element[n];
    for (int i = 0; i < tests_count; ++i) {
        std::copy(arr, arr + n, arr_copy);
        Timer t;
        func(arr_copy, n);
        result += t.Finish();
    }
    delete[] arr_copy;
    result /= tests_count * 1000;
    std::cout << result << std::endl;
    return result;
}

ms TimeIt(Element* begin, Element* end, sorting_range func, const string&& name) {
    common::test(func, true);
    std::cout << "sorting algo " << algo_number++ << ": ";
    if (show_name) {
        string display_name(name); display_name.resize(20);
        std::cout << display_name << " \t";
    }
    ms result = 0;
    ElementCounter size = end - begin;
    Element* arr_copy = new Element[size];
    for (int i = 0; i < tests_count; ++i) {
        std::copy(begin, end, arr_copy);
        Timer t;
        func(arr_copy, arr_copy + size);
        result += t.Finish();
    }
    delete[] arr_copy;
    result /= tests_count * 1000;
    std::cout << result << std::endl;
    return result;
}

template <class T>
ms TimeItStd(T* begin, T* end) {
    std::cout << "sorting std::sort \t";
    ms result = 0;
    ElementCounter n = end - begin;
    T* arr_copy = new T[n];
    for (int i = 0; i < tests_count; ++i) {
        std::copy(begin, end, arr_copy);
        Timer t;
        std::sort(arr_copy, arr_copy + n);
        result += t.Finish();
    }
    delete[] arr_copy;
    result /= tests_count * 1000;
    std::cout << result << std::endl;
    return result;
}

#endif // !TIMER_H_
