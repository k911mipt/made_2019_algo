/*
# 4. Порядковая статистика

## 4_1.
Реализуйте стратегию выбора опорного элемента "медиана трёх".
Функцию Partition реализуйте методом прохода двумя итераторами от начала массива к концу.

| Ограничение времени | 1 секунда                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 16Mb                             |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..10^9] размера n.
Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с индексом k ∈[0..n-1] в отсортированном массиве.
Напишите нерекурсивный алгоритм.
Требования к дополнительной памяти: O(n).
Требуемое среднее время работы: O(n).
Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.

###  Описание для случая прохода от начала массива к концу:

* Выбирается опорный элемент.
* Опорный элемент меняется с последним элементом массива.
* Во время работы Partition в начале массива содержатся элементы, не бОльшие опорного. Затем располагаются элементы, строго бОльшие опорного. В конце массива лежат нерассмотренные элементы. Последним элементом лежит опорный.
* Итератор (индекс) i указывает на начало группы элементов, строго бОльших опорного.
* Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
* Шаг алгоритма. Рассматривается элемент, на который указывает j. Если он больше опорного, то сдвигаем j. Если он не больше опорного, то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
* В конце работы алгоритма меняем опорный и элемент, на который указывает итератор i.

*/

#include <iostream>
#include <fstream>

#pragma region "partition.h"
#ifndef PARTITION_H_
#define PARTITION_H_

namespace made {

    namespace stl {

        typedef size_t size_type;

        template <class T>
        inline void CompareSwap(T* left, T* right) {
            if (*left > *right) {
                std::swap(*left, *right);
            }
        }

        template <class T>
        void ChoosePivotOfThree(T* first, T* last) {
            size_type pivot_index = (last - first) / 2;
            T* pivot = first + pivot_index;
            CompareSwap(pivot, last); // make (last >= mid)
            CompareSwap(last, first); // make (first >= last)
            CompareSwap(pivot, last); // make (first >= last >= mid)
        }

        template <class T>
        void InsertionSort(T* begin, T* end) {
            T* j;
            T* i = begin + 1;
            for (; i < end; ++i) {
                j = i - 1;
                while ((j >= begin) && (*j > *(j + 1))) {
                    std::swap(*j, *(j + 1));
                    --j;
                }
            }
        }

        /*
         * This function will find median of medians and put it at begin of given range
         * picking that median we guarantee a "good" division, so
         */
        template <class T>
        void MedianOfMedians(T* begin, T* end) {
            size_type length = end - begin;
            if (end - begin <= 5) {
                InsertionSort(begin, end);
                std::swap(*begin, *(begin + length / 2));
                return;
            }
            
            size_type parts_count = length / 5;
            T* left = begin;
            T* parts_median = begin + 2;
            T* medians_head = begin;
            for (size_type i = 0; i < parts_count; ++i) {
                InsertionSort(left, left + 5);
                std::swap(*parts_median, *medians_head);
                ++medians_head;
                parts_median += 5;
                left += 5;
            }
            MedianOfMedians(begin, medians_head);
        }

        template <class T>
        T* Partition(T* first, T* last) {
            size_type length = last - first + 1;
            if (length > 800) {
                MedianOfMedians(first, last + 1);
                std::swap(*first, *last);
            }
            else {
                ChoosePivotOfThree(first, last);
            }
            T pivot = *last;
            T* i = first;
            T* j = first;
            
            for (T* j = first; j < last; ++j) {
                if (!(*j > pivot)) {
                    std::swap(*i, *j);
                    ++i;
                }
            }
            std::swap(*i, *last);
            return i;
        }

        template <class T>
        T FindKOrderStatistic(T* begin, T* end, size_type k) {
            T* left = begin;
            T* right = end - 1;
            T* mid;
            size_type mid_index;
            while (true) {
                mid = Partition(left, right);
                mid_index = mid - begin;
                if (mid_index == k) {
                    return *mid;
                }
                else if (k < mid_index) {
                    right = mid - 1;
                }
                else {
                    left = mid + 1;
                }
            }
        }
    }
}
#endif // PARTITION_H_
#pragma endregion "partition.h"


using namespace made::stl;

inline void input(int *arr, size_type n) {
    for (size_type i = 0; i < n; ++i, ++arr) {
        std::cin >> *arr;
    }
}

inline void output(int *arr, size_type n) {
    for (size_type i = 0; i < n; ++i, ++arr) {
        std::cout << *arr << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_type n, k;
    std::cin >> n >> k;

    int* arr = new int[n];
    input(arr, n);
    std::cout << FindKOrderStatistic(arr, arr + n, k) << std::endl;

    delete[] arr;
    return EXIT_SUCCESS;
}
