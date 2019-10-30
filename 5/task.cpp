/*
# Задание 2_4. Скользящий максимум

| Ограничение времени | 20 секунд                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 300Mb                            |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Дан массив целых чисел в диапазоне [0..10^9].
Размер массива кратен 10 и ограничен сверху значением 2.5 * 10^7 элементов.
Все значения массива являются элементами псевдо-рандомной последовательности.
Необходимо написать функцию сортировки массива.

Для этого нужно прислать .cpp файл, в котором описать шаблонную функцию Sort

```c++
template <class T, class TLess>
void Sort(T* arr, int size, const TLess& isLess);
```
*/

#include <iostream>
#include <fstream>
#include <algorithm>

#include <cassert>
#include "common.h"
#include "timer.h"

#include "radix_sort.h"
#include "gorset.h"
#include "radix_extra_memory.h"
#include "custom_radix_extra_memory.h"
#include "one_pass_radix_extra_memory.h"
#include "three_way.h"


//#include "custom_sort.h"


void gorset_sort_adapter(size_type* arr, size_type n) {
    gorset::radix_sort(arr, 0, n, 24);
}

void radix_sort_adapter(size_type* arr, size_type n) {
    radix_sort::radix_sort(arr, size_type(0), n, radix_sort::shift_start - radix_sort::shift_step);
}

void three_way_adapter(size_type* arr, size_type n) {
    three_way::quicksort(arr, 0, n - 1);
}

using namespace common;

void test() {
    size_type n = 10000;
    size_type* arr = new size_type[n];
    size_type* arr_correct = new size_type[n];
    generate(arr, n);
    std::copy(arr, arr + n, arr_correct);
    std::sort(arr_correct, arr_correct + n);
    //Sort(arr, n);
    
    one_pass_radix_extra_memory::RadixSort(arr, n);
    //custom_radix_extra_memory::RadixSort(arr, n);
    /*radix_extra_memory::RadixSort(arr, n);*/
    //radix_sort_adapter(arr, n);
    //three_way::quicksort(arr, 0, n - 1);
    /*radix_sort::radix_sort(arr, 0, n, radix_sort::shift_start - radix_sort::shift_change);*/
    //RadixSort(arr, n);
    //custom_radix_sort::radix_sort(arr, arr + n);
    //QuickSort(arr, arr + n);
    for (size_type i = 0; i < n; ++i)
        assert(arr[i] == arr_correct[i]);
    delete[] arr;
    delete[] arr_correct;
}

int main() {
    tests_count = 1;
    test();
    size_type n = 25000;// 000;
    size_type* arr = new size_type[n];
    std::cout << "generating " << n << " randoms in range " << 0 << "-" << MAX_VALUE << std::endl;
    generate(arr, n);
    //output(arr, n);
    ms time;
    // прогреваемся
    //std::cout << "sorting" << std::endl;
    //time = TimeItStd(arr, arr + n);
    //std::cout << "average std::sort " << time / 1000 << std::endl;

    //std::cout << "sorting" << std::endl;
    //time = TimeItStd(arr, arr+ n);
    //std::cout << "average std::sort " << time / 1000 << std::endl;

    std::cout << "sorting" << std::endl;
    time = TimeIt(arr, n, gorset_sort_adapter);
    std::cout << "average gorset_sort " << time / 1000 << std::endl;
    //std::cout << "average algo1 " << time / 1000 << std::endl;

    std::cout << "sorting" << std::endl;
    time = TimeIt(arr, n, radix_sort_adapter);
    std::cout << "average custom_radix_sort " << time / 1000 << std::endl;
    //std::cout << "average algo2 " << time / 1000 << std::endl;

    std::cout << "sorting" << std::endl;
    time = TimeIt(arr, n, radix_extra_memory::RadixSort);
    std::cout << "average radix_extra_memory " << time / 1000 << std::endl;
    //std::cout << "average algo3 " << time / 1000 << std::endl;

    std::cout << "sorting" << std::endl;
    time = TimeIt(arr, n, one_pass_radix_extra_memory::RadixSort);
    std::cout << "average one_pass_radix_extra_memory " << time / 1000 << std::endl;
    //std::cout << "average algo4 " << time / 1000 << std::endl;

    std::cout << "sorting" << std::endl;
    time = TimeIt(arr, n, custom_radix_extra_memory::RadixSort);
    std::cout << "average custom_radix_extra_memory " << time / 1000 << std::endl;
    //std::cout << "average algo5 " << time / 1000 << std::endl;

    //std::cout << "sorting" << std::endl;
    //time = TimeIt(arr, n, three_way_adapter);
    //std::cout << "average three_way_sort " << time / 1000 << std::endl;
    //std::cout << "average algo6 " << time / 1000 << std::endl;

    delete[] arr;
    return 0;
}