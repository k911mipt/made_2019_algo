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
#include <random>
#include "custom_sort.h"
#include "timer.h"


//using made::stl::size_type;
const size_type MAX_VALUE = 1000000000;
//const size_type MAX_VALUE = 6;

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

int main() {
    size_type n = 1000;
    size_type* arr = new size_type[n];

    generate(arr, n);
    ms time;

    time = TimeIt(arr, n, Sort);
    std::cout << "average BubbleSort " << time << std::endl;
    time = TimeItStd(arr, arr + n);
    std::cout << "average std::sort " << time << std::endl;
    
    /*time = TimeIt2(arr, arr + n, std::sort<size_type*>);*/
    //time = TimeIt(arr, n, std::sort);

    //Sort(arr, n);
    std::sort(arr, arr + n);
    //output(arr, n);

    delete[] arr;
}