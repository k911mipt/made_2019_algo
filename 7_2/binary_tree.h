#pragma once
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
            TreeNode(T _value) :value(_value) {}
            const T value;
            //Tptr n_left() { return &left_; }
            //Tptr n_right() { return &left_; }
            //void SetLeft(Tptr node) { left_ = node; }
            //void SetRight(Tptr node) { right_ = node; }
        //private:
            TreeNode<T>* left = nullptr;
            TreeNode<T>* right = nullptr;
            //bool operator < (const TreeNode& rhs) const { return value < rhs.value; }
            //bool operator > (const TreeNode& rhs) const { return value > rhs.value; }
            //bool operator <= (const TreeNode& rhs) const { return value <= rhs.value; }
            //bool operator >= (const TreeNode& rhs) const { return value >= rhs.value; }
        };
#pragma endregion

#pragma region BinaryTree
        template<class T, class Node = TreeNode<T>>
        class BinaryTree {
        public:
            BinaryTree() = default;
            ~BinaryTree();
            virtual void Insert(T value);
            virtual Node* Search(T value);
            void Delete(T value);
            void GetPreOrderValues(T* arr);
        protected:
            typedef std::function<void(Node*)> BypassEvent;
            Node* root_ = nullptr;
            virtual Node* SearchParent(T value);
            void Delete(Node* node, Node* parent);

            Node* Merge(Node* left, Node* right);
            void BypassPreOrder(BypassEvent cb);
        };

        template<class T, class Node>
        BinaryTree<T, Node>::~BinaryTree() {
            BypassPreOrder([](Node* node) {
                delete node;
            });
        }

        template<class T, class Node>
        void BinaryTree<T, Node>::GetPreOrderValues(T* arr) {
            BypassPreOrder([&arr](const Node* node) {
                *arr++ = node->value;
            });
        }

        template<class T, class Node>
        void BinaryTree<T, Node>::Insert(T value) {
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