﻿/*
# Задание 7_2. Солдаты Сплей

| Ограничение времени | 1 секунда                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 500Mb                            |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

В одной военной части решили построить в одну шеренгу по росту.
Т.к. часть была далеко не образцовая, то солдаты часто приходили не вовремя,
а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены
по росту – сначала самые высокие, а в конце – самые низкие. За расстановку
солдат отвечал прапорщик, который заметил интересную особенность – все солдаты
в части разного роста.

Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
а именно для каждого приходящего солдата указывать, перед каким солдатом в строе
он должен становится. Требуемая скорость выполнения команды - O(log n) амортизационно.

В реализации используйте сплей деревья


## Формат ввода

Первая строка содержит число N – количество команд (1 ≤ N ≤ 90 000).
В каждой следующей строке содержится описание команды:

* число 1 и X если солдат приходит в строй (X – рост солдата, натуральное число до 100 000 включительно)
* число 2 и Y если солдата, стоящим в строе на месте Y надо удалить из строя.

Солдаты в строе нумеруются с нуля

## Формат вывода

На каждую команду 1 (добавление в строй) вы должны выводить
число K – номер позиции, на которую должен встать этот солдат (все стоящие за ним двигаются назад).

*/

#include <iostream>
#include <fstream>
#include "memcheck_crt.h"
#include "splay_tree.h"

using namespace made::stl;
using val_t = int;

int main() {
    ENABLE_CRT;
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    size_t n = 0;
    std::cin >> n;

    SplayTree<val_t> tree;
    val_t val = 0;
    int number = 0;
    for (size_t i = 0; i < n; ++i) {
        std::cin >> number >> val;
        if (number > 0) {
            tree.Insert(number);
            std::cout << tree.FindKOrder(val) << " ";
        }
        else {
            tree.Delete(-number);
            std::cout << tree.FindKOrder(val) << " ";
        }
    }
    return EXIT_SUCCESS;
}