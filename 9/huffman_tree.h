#pragma once
#ifndef HUFFMAN_TREE_H_
#define HUFFMAN_TREE_H_

#include <functional>
#include <queue>
#include <vector>
#include <cassert>

#include "bit_stream.h"

namespace made {

    namespace huffman {
        
        using made::stl::BitsReader;
        using made::stl::BitsWriter;

        class Node {
            using BypassEvent = std::function<void(Node*)>;
        public:
            Node(byte _code = 0, unsigned _amount = 0);
            Node(Node* _left, Node* _right);
            const bool operator<(const Node& rhs) const&;
            void BypassPreOrder(BypassEvent cb);
            void Clear();
            void FillTableSet(std::unordered_map<byte, std::vector<bool>>& set, std::vector<bool>& bits);
            void WriteTree(BitsWriter &stream);
            Node* ReadTree(BitsReader &stream);
        private:
            unsigned amount = 0;
        public:// TODO make private
            byte code = 0;
            Node * left = nullptr;
            Node * right = nullptr;
        };

        Node::Node(byte _code, unsigned _amount) :
            code(_code),
            amount(_amount)
        {}

        Node::Node(Node* _left, Node* _right) :
            left(_left),
            right(_right),
            amount(_left->amount + _right->amount)
        {}

        const bool Node::operator<(const Node& rhs) const& {
            return amount > rhs.amount;
        }

        void Node::BypassPreOrder(BypassEvent cb) {
            std::vector<Node*> stack;
            Node* node = this;
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

        void Node::Clear() {
            BypassPreOrder([](Node* node) {
                delete node;
            });
        }

        void Node::FillTableSet(std::unordered_map<byte, std::vector<bool>>& set, std::vector<bool>& bits) {
            if (left) {
                assert(right);
                bits.push_back(0);
                left->FillTableSet(set, bits);
                bits.pop_back();
                bits.push_back(1);
                right->FillTableSet(set, bits);
                bits.pop_back();
            }
            else {
                assert(!right);
                set[code] = bits;
            }
        }

        void Node::WriteTree(BitsWriter &stream) {
            if (left) {
                stream.WriteBit(1);
                left->WriteTree(stream);
                right->WriteTree(stream);
            }
            else {
                stream.WriteBit(0);
                stream.WriteByte(code);
            }
        }

        Node* Node::ReadTree(BitsReader &stream) {
            bool bit = stream.ReadBit();
            if (bit) {
                left = (new Node())->ReadTree(stream);
                right = (new Node())->ReadTree(stream);
            }
            else {
                code = stream.ReadByte();
            }
            return this;
        }

        Node* BuildHuffmanTree(unsigned frequences[256]) {
            // Build queue of single nodes
            std::priority_queue<Node> nodes;
            for (unsigned i = 0; i < 256; ++i) {
                auto count = frequences[i];
                if (count)
                    nodes.emplace(i, count);
            }
            // Combine a tree of these nodes
            while (nodes.size() > 1) {
                Node *l = new Node(nodes.top());
                nodes.pop();
                Node *r = new Node(nodes.top());
                nodes.pop();
                nodes.emplace(l, r);
            }
            return new Node(nodes.top());
        }
    }
}

#endif // !HUFFMAN_TREE_H_