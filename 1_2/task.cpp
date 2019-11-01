/*
# Задание 1_2. Дек с динамическим буфером

| Ограничение времени | 1 секунда                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 5Mb                              |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Реализовать дек с динамическим зацикленным буфером.
Обрабатывать команды `push *` и `pop *`.

## Формат ввода

В первой строке количество команд *`n`*. *`n ≤ 1000000`*.

Каждая команда задаётся как 2 целых числа: a b.

* a = 1 - push front
* a = 2 - pop front
* a = 3 - push back
* a = 4 - pop back

Если дана команда `pop *`, то число `b` - ожидаемое значение. Если команда `pop *` вызвана для пустой структуры данных, то ожидается `-1`.

Амортизированное время добавления и удаления элемента: O(1)
Потребляемая память O(n)
*/

#include <iostream>
#include <fstream>
#include "deque.h"

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int n = 0;
    std::cin >> n;
    made::stl::Deque<int> dequeue;
    int operation = 0;
    int value = 0;
    for (int i = 0; i < n; i++) {
        std::cin >> operation >> value;
        switch (operation) {
        case 1:
            dequeue.PushFront(value);
            break;
        case 2:
            if (dequeue.IsEmpty()) {
                if (-1 != value) {
                    std::cout << "NO" << std::endl;
                    return EXIT_SUCCESS;
                }
                break;
            }
            else if (value != dequeue.PopFront()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        case 3:
            dequeue.PushBack(value);
            break;
        case 4:
            if (dequeue.IsEmpty()) {
                if (-1 != value) {
                    std::cout << "NO" << std::endl;
                    return EXIT_SUCCESS;
                }
                break;
            }
            else if (value != dequeue.PopBack()) {
                std::cout << "NO" << std::endl;
                return EXIT_SUCCESS;
            }
            break;
        }
    }
    std::cout << "YES" << std::endl;
    return EXIT_SUCCESS;
}