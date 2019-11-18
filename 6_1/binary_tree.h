#pragma once
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
