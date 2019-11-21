#pragma once
#ifndef SPLAY_TREE_H_
#define SPLAY_TREE_H_

#include <vector>
#include <functional>
#include <cassert>

namespace made {

    namespace stl {

#pragma region SplayTreeNode
        template<class T>
        struct SplayTreeNode {
            using Node = SplayTreeNode<T>;
            SplayTreeNode(const T& _value) :value(_value) {}
            size_t GetRightSize() { return (right ? right->size : 0); }
            size_t GetLeftSize() { return (left ? left->size : 0); }
            void UpdateSize();
            void IncLeftSizeChain(size_t additional);
            void IncRightSizeChain(size_t additional);
            Node* RotateLeft();
            Node* RotateRight();
            Node* SplitLeft(Node* &sub_node, Node* &root);
            Node* SplitRight(Node* &sub_node, Node* &root);

            const T value;
            size_t size = 1;
            Node* left = nullptr;
            Node* right = nullptr;
        };

        template<class T>
        void SplayTreeNode<T>::UpdateSize() {
            size = 1;
            size += left ? left->size : 0;
            size += right ? right->size : 0;
        }

        template<class T>
        void SplayTreeNode<T>::IncLeftSizeChain(size_t additional) {
            size += additional;
            Node* node = left;
            while (node) {
                node->size += additional;
                node = node->left;
            }
        }

        template<class T>
        void SplayTreeNode<T>::IncRightSizeChain(size_t additional) {
            size += additional;
            Node* node = right;
            while (node) {
                node->size += additional;
                node = node->right;
            }
        }

        template<class T>
        SplayTreeNode<T>* SplayTreeNode<T>::RotateLeft() {
            Node* temp = right;
            right = right->left;
            temp->left = this;
            UpdateSize();
            temp->UpdateSize();
            return temp;
        }

        template<class T>
        SplayTreeNode<T>* SplayTreeNode<T>::RotateRight() {
            Node* temp = left;
            left = left->right;
            temp->right = this;
            UpdateSize();
            temp->UpdateSize();
            return temp;
        }

        template<class T>
        SplayTreeNode<T>* SplayTreeNode<T>::SplitLeft(Node* &sub_node, Node* &root) {
            Node* tmp = right;
            right = nullptr;
            UpdateSize();
            if (!sub_node)
                root = sub_node = this;
            else {
                root->IncRightSizeChain(size);
                sub_node->right = this;
                sub_node = this;
            }
            return tmp;
        }

        template<class T>
        SplayTreeNode<T>* SplayTreeNode<T>::SplitRight(Node* &sub_node, Node* &root) {
            Node* tmp = left;
            left = nullptr;
            UpdateSize();
            if (!sub_node)
                root = sub_node = this;
            else {
                root->IncLeftSizeChain(size);
                sub_node->left = this;
                sub_node = this;
            }
            return tmp;
        }
#pragma endregion

#pragma region SplayTree
        template<class T, class Comparator = std::less<>>
        class SplayTree {
            using Node = SplayTreeNode<T>;
        public:
            SplayTree() = default;
            ~SplayTree();
            size_t GetSize() { return root_ ? root_->size : 0; }
            size_t Insert(const T& value);
            size_t InsertSplay(const T& value);
            T FindKOrder(const size_t pos, Node* root = nullptr);
            void Delete(const T& value);
            void DeletePos(const size_t pos);
        private:
            typedef std::function<void(Node*)> BypassEvent;
            Node* root_ = nullptr;
            Comparator comp_;
        private:
            Node* Splay(const T& value, Node* root = nullptr);
            Node* Merge(Node* left, Node* right);
            Node* MergeSimple(Node* left, Node* right);
            void BypassPreOrder(BypassEvent cb);
        };

        template<class T, class Comparator>
        SplayTree<T, Comparator>::~SplayTree() {
            BypassPreOrder([](Node* node) {
                delete node;
            });
        }

        /// <summary>Insert like in a common binary tree, but update node weights and splay a new node afterwards</summary>
        template<class T, class Comparator>
        size_t SplayTree<T, Comparator>::Insert(const T& value) {
            Node* leaf = new Node(value);
            Node* parent = root_;
            if (!parent) {
                root_ = leaf;
                return 0;
            }
            Node* temp_parent = parent;
            size_t pos = parent->size;
            while (parent) {
                bool less = comp_(value, parent->value);
                temp_parent = less ? parent->left : parent->right;
                if (less) {
                    parent->left = parent->left ? parent->left : leaf;
                    pos -= (parent->GetRightSize() + 1);
                }
                else {
                    parent->right = parent->right ? parent->right : leaf;
                }
                parent->size++;
                parent = temp_parent;
            }
            Splay(value);
            return pos;
        }

        /// <summary>Insert with inplace top-down splay</summary>
        template<class T, class Comparator>
        size_t SplayTree<T, Comparator>::InsertSplay(const T& value) {
            Node* leaf = new Node(value);
            root_ = root_ ? root_ : leaf;
            Node* &t = root_;
            Node *l_root = nullptr, *r_root = nullptr, *l_leaf = nullptr, *r_leaf = nullptr;
            while (t && t->value != value) {
                if (comp_(value, t->value)) {
                    t->left = t->left ? t->left : leaf;
                    if (comp_(value, t->left->value)) {
                        t = t->RotateRight();
                    }
                    t = t->SplitRight(r_leaf, r_root);
                }
                else {
                    t->right = t->right ? t->right : leaf;
                    if (comp_(t->right->value, value)) {
                        t = t->RotateLeft();
                    }
                    t = t->SplitLeft(l_leaf, l_root);
                }
                t = t ? t : leaf;
            }
            t->left = l_root;
            t->right = r_root;
            t->UpdateSize();
            return t->GetLeftSize();
        }

        /// <summary>Find K statitics value</summary>
        template<class T, class Comparator>
        T SplayTree<T, Comparator>::FindKOrder(const size_t pos, Node* root) {
            root = root ? root : root_;
            assert(root && (root->size > pos));
            Node* node = root;
            Node* parent = nullptr;
            size_t seek_pos = node->GetLeftSize();
            while (pos != seek_pos) {
                parent = node;
                if (pos > seek_pos) {
                    assert(node->right);
                    node = node->right;
                    seek_pos += node->GetLeftSize() + 1;
                }
                else if (pos < seek_pos) {
                    assert(node->left);
                    node = node->left;
                    seek_pos -= (node->GetRightSize() + 1);
                }
            }
            return node->value;
        }

        /// <summary>Splay a value to root. Split and merge its childen into a new root</summary>
        template<class T, class Comparator>
        void SplayTree<T, Comparator>::Delete(const T& value) {
            Node* node = Splay(value);
            root_ = Merge(node->left, node->right);
            delete node;
        }

        /// <summary>Inplace seeking and deleting K-ordered value w/o rebalancing</summary>
        template<class T, class Comparator>
        void SplayTree<T, Comparator>::DeletePos(const size_t pos) {
            if (!root_) {
                return;
            }
            Node* node = root_;
            Node* parent = nullptr;
            size_t seek_pos = node->GetLeftSize();
            bool last_left = false;
            while (node && (pos != seek_pos)) {
                parent = node;
                node->size--;
                if (pos > seek_pos) {
                    node = node->right;
                    if (!node) return;
                    seek_pos += node->GetLeftSize() + 1;
                    last_left = false;
                }
                else if (pos < seek_pos) {
                    node = node->left;
                    if (!node) return;
                    seek_pos -= (node->GetRightSize() + 1);
                    last_left = true;
                }
            }
            if (pos == seek_pos) {
                Node* new_node = MergeSimple(node->left, node->right);
                root_ = (node == root_) ? new_node : root_;
                delete node;
                if (parent) {
                    if (last_left) {
                        parent->left = new_node;
                    }
                    else {
                        parent->right = new_node;
                    }
                    parent->UpdateSize();
                }
            }
        }

        /// <summary>Find node with given value and pop it to become a new root</summary>
        template<class T, class Comparator>
        SplayTreeNode<T>* SplayTree<T, Comparator>::Splay(const T& value, Node* root) {
            Node* &t = root ? root : root_;
            Node *l_root = nullptr, *r_root = nullptr, *l_leaf = nullptr, *r_leaf = nullptr;
            while (t && t->value != value) {
                if (comp_(value, t->value)) {
                    if (comp_(value, t->left->value)) {
                        t = t->RotateRight();
                    }
                    t = t->SplitRight(r_leaf, r_root);
                }
                else {
                    if (comp_(t->right->value, value)) {
                        t = t->RotateLeft();
                    }
                    t = t->SplitLeft(l_leaf, l_root);
                }
            }
            if (l_leaf) {
                l_root->IncRightSizeChain(t->GetLeftSize());
                l_leaf->right = t->left;
                t->left = l_root;
            }
            if (r_leaf) {
                r_root->IncLeftSizeChain(t->GetRightSize());
                r_leaf->left = t->right;
                t->right = r_root;
            }
            t->UpdateSize();
            return t;
        }

        /// <summary>Find maximum in left tree, splay it, and join right tree to it</summary>
        template<class T, class Comparator>
        SplayTreeNode<T>* SplayTree<T, Comparator>::Merge(Node* n_left, Node* n_right) {
            if (!n_left)
                return n_right;
            if (!n_right)
                return n_left;
            size_t pos = n_left->size - 1;
            T elem = FindKOrder(pos, n_left);
            n_left = Splay(elem, n_left);
            n_left->right = n_right;
            n_left->UpdateSize();
            return n_left;
        }

        /// <summary>Find maximum in left tree, join right tree to it. No rebalancing</summary>
        template<class T, class Comparator>
        SplayTreeNode<T>* SplayTree<T, Comparator>::MergeSimple(Node* n_left, Node* n_right) {
            if (!n_left)
                return n_right;
            if (!n_right)
                return n_left;
            size_t additional_size = n_right->size;
            Node* node = n_left;
            while (node->right) {
                node->size += additional_size;
                node = node->right;
            }
            node->right = n_right;
            node->UpdateSize();
            return n_left;
        }

        /// <summary>Bypass nodes with given callback</summary>
        template<class T, class Comparator>
        inline void SplayTree<T, Comparator>::BypassPreOrder(BypassEvent cb) {
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

#endif // !SPLAY_TREE_H_
