#pragma once
#ifndef HUFFMAN_ENCODER_H_
#define HUFFMAN_ENCODER_H_

#include <vector>
#include <unordered_map>
//#include <queue>
#include <iostream>

#include "bit_stream.h"
#include "huffman_tree.h"

namespace made {

    namespace huffman {

        std::vector<byte> EncodeBytes(std::vector<byte>& input) {
            // Calculate frequences
            unsigned frequences[256] = { 0 };
            for (byte value : input) {
                frequences[value]++;
            }

            // Build a Huffman tree
            Node* root = BuildHuffmanTree(frequences);
            std::vector<bool> bits;
            if (!root->left) {
                // TODO : may optimize this situation for counting compressing
                bits.push_back(0);
            }

            // Write tree into stream
            BitsWriter out;
            root->WriteTree(out);

            // Create a hashmap {symbol : encoded but sequence}
            std::unordered_map<byte, std::vector<bool>> set;
            root->FillTableSet(set, bits);

            // Write encoded bits into stream
            root->Clear();
            int count = 0;
            for (byte symbol : input) {
                for (bool bit : set[symbol]) {
                    out.WriteBit(bit);
                    ++count;
                }
            }

            bool encoded = true;
            std::vector<byte> output = std::move(out.GetResult());
            if (output.size() > input.size()) {
                output = input;
                encoded = false;
                output.push_back(0);
            }
            // put ENCODED flag into 4th bit of last byte
            output.back() = output.back() | (encoded << 3);
            return output;
        }

        std::vector<byte> DecodeTree(Node* root, BitsReader& in) {
            std::vector<byte> output;
            if (root->left) {
                while (in) {
                    Node* node = root;
                    while (node->left) {
                        bool bit = in.ReadBit();
                        node = bit ? node->right : node->left;
                    }
                    output.push_back(node->code);
                    //std::cout << node->code;// << " ";
                }
            }
            else {
                while (in) {
                    in.ReadBit(); // TODO : optimize with getting stream size
                    output.push_back(root->code);
                    //std::cout << root->code;
                }
            }
            return output;
        }

        std::vector<byte> DecodeBytes(std::vector<byte>& input) {
            bool encoded = (input.back() & 0x8) >> 3;
            input.back() = input.back() & 0x7;

            BitsReader in(std::move(input));
            if (encoded) {
                Node* root = (new Node())->ReadTree(in);
                std::vector<byte> output = DecodeTree(root, in);
                root->Clear();
                return output;
            }
            else {
                input.pop_back();
                return input;
            }
        }
    }
}
#endif // !HUFFMAN_ENCODER_H_