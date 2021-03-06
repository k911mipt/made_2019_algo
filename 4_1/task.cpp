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
#include "partition.h"

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