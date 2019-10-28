/*
# Задание 2_4. Скользящий максимум

| Ограничение времени | 2 секунды                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 5Mb                              |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Дан массив целых чисел `A[0..n)`, `n` не превосходит `100 000`.
Также задан размер некоторого окна (последовательно расположенных элементов массива) в этом массиве `k`, `k <= n`.
Требуется для каждого положения окна (от `0` и до `n-k`) вывести значение максимума в окне.
Скорость работы `O(n log n)`, память `O(n)`.

## Формат ввода

Вначале вводится `n` - количество элементов массива. Затем вводится `n` строк со значением каждого элемента. Затем вводится `k` - размер окна.

## Формат вывода

Разделенные пробелом значения максимумов для каждого положения окна.
*/

#include <iostream>
#include <fstream>
#include "solution.h"

using namespace made::solution;

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_type n;
    std::cin >> n;
    WindowElement* arr = new WindowElement[n];
    try {
        for (size_type i = 0; i < n; i++) {
            arr[i].index = i;
            std::cin >> arr[i].value;
        }
        size_type window_size;
        std::cin >> window_size;
        size_type answers_size = n - window_size + 1;
        int* answers = new int[answers_size];
        try {
            SolveWithHeap(arr, n, window_size, answers);
            for (size_type i = 0; i < answers_size; i++) {
                std::cout << answers[i] << " ";
            }
        }
        catch (...) {
            delete answers;
            throw;
        }
        delete answers;
    }
    catch (...) {
        delete arr;
        throw;
    }
    delete arr;
    return EXIT_SUCCESS;
}