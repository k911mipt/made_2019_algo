
#ifndef TIMER_H_
#define TIMER_H_
#include <chrono>
#include <iostream>
#include <functional>
//#include "custom_sort.h"

int tests_count = 1;

//template <class T>

typedef void(*sorting_size)(size_type* arr, size_type n);
typedef void(*sorting_range)(size_type* begin, size_type* end);
//typedef void(*sorting_range)(size_type *_First, size_type *_Last);
//typedef void(*std_sort)(size_type* begin, size_type* end);
//typedef std::function<size_type*>(*std_sort)(size_type *_First, size_type *_Last);

typedef long long ms;


class Timer
{
    using clock_t = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
public:
    Timer() {
        //std::cout << "Started timer" << std::endl;
        start_ = clock_t::now();
    }

    ~Timer() {

    }
    auto Finish() {
        const auto finish = clock_t::now();
        const auto us =
            std::chrono::duration_cast<microseconds>
            (finish - start_).count();
        //std::cout << us << " us" << std::endl;
        return us;
    }

private:
    clock_t::time_point start_;
};

ms TimeIt(size_type* arr, size_type n, sorting_size func) {
    ms result = 0;
    size_type* arr_copy = new size_type[n];
    for (int i = 0; i < tests_count; ++i) {
        std::copy(arr, arr + n, arr_copy);
        Timer t;
        func(arr_copy, n);
        result += t.Finish();
    }
    delete[] arr_copy;
    return result / tests_count;
}
inline void output2(size_type *arr, size_type n) {
    for (size_type i = 0; i < n; ++i, ++arr) {
        std::cout << *arr << " ";
    }
    std::cout << std::endl;
}

ms TimeIt(size_type* begin, size_type* end, sorting_range func) {
    ms result = 0;
    size_type n = end - begin;
    for (int i = 0; i < tests_count; ++i) {
        size_type* arr_copy = new size_type[n];
        std::copy(begin, end, arr_copy);
        Timer t;
        func(arr_copy, arr_copy + n);
        
        result += t.Finish();
        //output2(arr_copy, n);
    }
    return result / tests_count;
}

template <class T>
ms TimeItStd(T* begin, T* end) {
    ms result = 0;
    size_type n = end - begin;
    T* arr_copy = new T[n];
    for (int i = 0; i < tests_count; ++i) {
        std::copy(begin, end, arr_copy);
        Timer t;
        std::sort(arr_copy, arr_copy + n);
        result += t.Finish();
    }
    delete[] arr_copy;
    return result / tests_count;
}






#endif // !TIMER_H_
