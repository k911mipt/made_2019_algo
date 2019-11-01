/*
# Задание 3_2. Сортировка почти упорядоченной последовательности

| Ограничение времени | 2 секунды                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 20Mb                             |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Дана последовательность целых чисел a1...an и натуральное число k, такое что для любых i, j: если j >= i + k, то a[i] <= a[j].
Требуется отсортировать последовательность. Последовательность может быть очень длинной. 
Время работы O(n * log(k)). Доп. память O(k). 
Использовать слияние.
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "merge_sort.h"

using namespace made::stl;

inline void input(int *arr, size_type &i, size_type &j, size_type k, size_type n) {
    for (j = 0; j < std::min(k, n - i); ++j) {
        std::cin >> arr[j];
    }
    i += j;
}

inline void output(int *arr, size_type number) {
    for (size_type i = 0; i < number; ++i) {
        std::cout << arr[i] << " ";
    }
}

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    size_type n, k;
    std::cin >> n >> k;
    
    size_type capacity = k * 2;
    int* arr = new int[capacity];
    int* buffer = new int[capacity];
    int* arr_mid = arr + k;
    int* arr_end = arr + capacity;
    size_type i = 0;
    size_type j = 0;
    input(arr, i, j, k, n); // input left half [0..k-1]
    MergeSort(arr, arr + j, buffer);
    while (i < n) {
        input(arr_mid, i, j, k, n); // input right half [k..2*k]
        MergeSort(arr_mid, arr_mid + j, buffer); // sorting only right half
        Merge(arr, arr_mid, arr_mid + j, buffer); // merging left and right
        //output(arr, k); // output left half [0..k]
        //std::copy(arr_mid, arr_end, arr); // move right half to left half [k..2*k] -> [0..k]
    }
    output(arr, k); // output remainder of left half [0..j]
    delete[] buffer;
    delete[] arr;
    return EXIT_SUCCESS;
}