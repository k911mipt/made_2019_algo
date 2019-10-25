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

using made::stl::size_type;

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_type n;
    std::cin >> n;
    int* arr = new int[n];
    try {
        for (size_type i = 0; i < n; i++) {
            std::cin >> arr[i];
        }
        size_type window_size;
        std::cin >> window_size;

        size_type answers_size = n - window_size + 1;
        int* answers = new int[answers_size];
        try {
            // https://contest.yandex.ru/contest/14656/run-report/23142719/
            // This solution takes 18ms and 536Kb memory on 11 test, which is better than heap solution (22ms and 904Kb)
            // Though it has O(n^2) complexity and should not pass time limits if given array looks like `a[i] = n - i`
            //made::solution::BetterBenchmark(arr, n, window_size, answers);

            made::solution::SolveWithHeap(arr, n, window_size, answers);
            //made::solution::RMQSolution(arr, n, window_size, answers);
            
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