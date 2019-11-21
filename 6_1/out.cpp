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

#pragma region "binary_tree.h"
#ifndef BINARY_TREE_H_
#define BINARY_TREE_H_

#include <vector>
#include <functional>


namespace made {

    namespace stl {

        template<class T>
        class BinaryTree {
        private:
            struct TreeNode {
                TreeNode(T init_value) :value(init_value) {}
                T value;
                TreeNode* left = nullptr;
                TreeNode* right = nullptr;

                bool operator < (const TreeNode& rhs) {
                    return value < rhs.value;
                }
                bool operator > (const TreeNode& rhs) {
                    return value > rhs.value;
                }
                bool operator <= (const TreeNode& rhs) {
                    return value <= rhs.value;
                }
                bool operator >= (const TreeNode& rhs) {
                    return value >= rhs.value;
                }
            };
        public:
            BinaryTree() = default;
            ~BinaryTree() {
                BypassPreOrder([](TreeNode* node) { 
                    delete node; 
                });
            }

            void Insert(T value) {
                TreeNode* leaf = new TreeNode(value);
                if (!root_) {
                    root_ = leaf;
                    return;
                }
                TreeNode* parent = root_;
                TreeNode* temp_parent = root_;
                while (parent) {
                    if (*leaf < *parent) {
                        temp_parent = parent->left;
                        parent->left = parent->left ? parent->left : leaf;
                    }
                    else {
                        temp_parent = parent->right;
                        parent->right = parent->right ? parent->right : leaf;
                    }
                    parent = temp_parent;
                }
            }

            void GetPreOrderValues(T* arr) {
                BypassPreOrder([&arr](TreeNode* node) { 
                    *arr++ = node->value; 
                });
            }
        private:
            TreeNode* root_ = nullptr;

            typedef std::function<void(TreeNode*)> BypassEvent;

            void BypassPreOrder(BypassEvent cb) {
                if (!root_) {
                    return;
                }
                std::vector<TreeNode*> stack;
                TreeNode* node = root_;
                while (node) {
                    if (node->left && node->right) {
                        stack.push_back(node->right);
                    }
                    TreeNode* temp_node = node->left ? node->left : node->right;
                    cb(node);
                    node = temp_node;
                    if (!node && stack.size()) {
                        node = stack.back();
                        stack.pop_back();
                    }
                }
            }
        };

    }

}

#endif // BINARY_TREE_H_
#pragma endregion "binary_tree.h"


#ifdef WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace made::stl;
using val_t = int;

int main() {
#ifdef WINDOWS
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif // WINDOWS
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

    val_t* arr = new val_t[n];
    tree.GetPreOrderValues(arr);
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << " ";
    }
    delete(arr);
    return EXIT_SUCCESS;
}
