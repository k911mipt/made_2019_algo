/*
# Задание 6_1.  Порядок обхода

| Ограничение времени | 1 секунда                        |
|---------------------|----------------------------------|
| Ограничение памяти  | 5Mb                              |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Дано число N < 10^6 и последовательность целых чисел из [-2^31..2^31] длиной N.
Требуется построить бинарное дерево, заданное наивным порядком вставки.
Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, 
то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
Рекурсия запрещена.

*/

#include <iostream>
#include <fstream>
#include "memcheck_crt.h"
#include "binary_tree.h"

using namespace made::stl;
using val_t = int;

int main() {
    ENABLE_CRT;
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int n = 0;
    std::cin >> n;

    BinaryTree<val_t> tree;
    val_t val = 0;
    for (int i = 0; i < n; ++i) {
        std::cin >> val;
        tree.Insert(val);
    }

    std::vector<val_t> container;
    tree.GetPreOrderValues(container);
    for (auto elem : container) {
        std::cout << elem << " ";
    }

    return EXIT_SUCCESS;
}