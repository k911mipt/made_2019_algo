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

#pragma region "memcheck_crt.h"
#ifndef MEMCHECK_CRT_
#define MEMCHECK_CRT_

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define ENABLE_CRT\
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);\
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);\
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#else
#define ENABLE_CRT ;
#endif

#endif // !MEMCHECK_CRT_
#pragma endregion "memcheck_crt.h"

#pragma region "binary_tree.h"
#ifndef BINARY_TREE_H_
#define BINARY_TREE_H_

#include <vector>
#include <functional>

namespace made {

    namespace stl {

#pragma region TreeNode
        template<class T>
        class TreeNode {
            using Tptr = TreeNode<T>*;
        public:
            TreeNode(const T& _value) :value(_value) {}
            const T value;
            TreeNode<T>* left = nullptr;
            TreeNode<T>* right = nullptr;
        };
#pragma endregion

#pragma region BinaryTree
        template<class T, class Node = TreeNode<T>>
        class BinaryTree {
        public:
            BinaryTree() = default;
            ~BinaryTree();
            void Insert(const T& value);
            void Delete(T value);
            void GetPreOrderValues(T* arr);
            void GetPreOrderValues(std::vector<T>& container);
        protected:
            typedef std::function<void(Node*)> BypassEvent;
            Node* root_ = nullptr;
            Node* Search(T value);
            Node* SearchParent(T value);
            void Delete(Node* node, Node* parent);
            Node* Merge(Node* left, Node* right);
            void BypassPreOrder(BypassEvent cb);
        };

        template<class T, class Node>
        inline BinaryTree<T, Node>::~BinaryTree() {
            BypassPreOrder([](Node* node) {
                delete node;
            });
        }

        template<class T, class Node>
        inline void BinaryTree<T, Node>::GetPreOrderValues(T* arr) {
            BypassPreOrder([&arr](const Node* node) {
                *arr++ = node->value;
            });
        }

        template<class T, class Node>
        inline void BinaryTree<T, Node>::GetPreOrderValues(std::vector<T> & container) {
            BypassPreOrder([&container](const Node* node) {
                container.push_back(node->value);
            });
        }

        template<class T, class Node>
        void BinaryTree<T, Node>::Insert(const T& value) {
            Node* leaf = new Node(value);
            if (!root_) {
                root_ = leaf;
                return;
            }
            Node* parent = root_;
            Node* temp_parent = root_;
            while (parent) {
                const T& pvalue = parent->value;
                temp_parent = (value < pvalue) ? parent->left : parent->right;
                if (value < pvalue) {
                    parent->left = parent->left ? parent->left : leaf;
                }
                else {
                    parent->right = parent->right ? parent->right : leaf;
                }
                parent = temp_parent;
            }
        }

        template<class T, class Node>
        Node* BinaryTree<T, Node>::Search(T value) {
            Node* parent = SearchParent(value);
            if (!parent) {
                return root_;
            }
            Node* node = value < parent->value ? parent->left : parent->right;
            return node->value == value ? node : nullptr;
        }

        template<class T, class Node>
        Node* BinaryTree<T, Node>::SearchParent(T value) {
            Node* node = root_;
            Node* parent = nullptr;
            while (node && (node->value != value)) {
                parent = node;
                node = value < node->value ? node->left : node->right;
            }
            return parent;
        }

        template<class T, class Node>
        void BinaryTree<T, Node>::Delete(T value) {
            Node* parent = SearchParent(value);
            Node* node = value < parent->value ? parent->left : parent->right;
            if (node->value != value) {
                return;
            }
            Delete(node, parent);
        }

        template<class T, class Node>
        void BinaryTree<T, Node>::Delete(Node* node, Node* parent) {
            Node* new_node = Merge(node->left, node->right);
            parent->left = parent->left == node ? new_node : parent->left;
            parent->right = parent->right == node ? new_node : parent->right;
            delete node;
        }

        template<class T, class Node>
        Node* BinaryTree<T, Node>::Merge(Node* n_left, Node* n_right) {
            if (!n_left)
                return n_right;
            if (!n_right)
                return n_left;
            Node* node = n_right->left;
            if (!node) {
                n_right->left = n_left;
                return n_right;
            }
            Node* parent = n_right;
            while (node && node->left) {
                parent = node;
                node = node->left;
            }
            parent->left = node->right;
            node->right = n_right;
            node->left = n_left;
            return node;
        }

        template<class T, class Node>
        inline void BinaryTree<T, Node>::BypassPreOrder(BypassEvent cb) {
            if (!root_) {
                return;
            }
            std::vector<Node*> stack;
            Node* node = root_;
            while (node) {
                if (node->left && node->right) {
                    stack.push_back(node->right);
                }
                Node* temp_node = node->left ? node->left : node->right;
                cb(node);
                node = temp_node;
                if (!node && stack.size()) {
                    node = stack.back();
                    stack.pop_back();
                }
            }
        }
#pragma endregion

    }
}

#endif // !BINARY_TREE_H_
#pragma endregion "binary_tree.h"

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
