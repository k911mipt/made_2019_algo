#include <vector>
#include <functional>

#include "Huffman.h"

#pragma region "huffman_encoder.h"
#ifndef HUFFMAN_ENCODER_H_
#define HUFFMAN_ENCODER_H_

#include <unordered_map>
#include <iostream>


#pragma region "bit_stream.h"
#ifndef BIT_STREAM_H_
#define BIT_STREAM_H_


namespace made {

    namespace stl {
        using byte = unsigned char;
        inline constexpr byte operator "" _byte(unsigned long long arg) noexcept {
            return static_cast<byte>(arg);
        }

        const byte STREAM_ENCODED_MASK = 0b00000001_byte;
        const byte LAST_BITS_FILLED_MASK = 0b00001110_byte;
        const byte LAST_BITS_START_POSITION = 1;

        class BitsWriter {
        public:
            void WriteBit(bool bit);
            void WriteByte(byte byte);
            std::vector<byte> GetResult();
        private:
            std::vector<byte> buffer_; 
            byte accumulator_ = STREAM_ENCODED_MASK; // First bit indicates that stream is encoded
            byte bits_count_ = 4;
        };

        void BitsWriter::WriteBit(bool bit) {
            // Ставим бит в аккумулятор на нужное место
            accumulator_ |= static_cast<byte>(bit) << bits_count_;
            ++bits_count_;
            if (bits_count_ == 8) {
                bits_count_ = 0;
                buffer_.push_back(accumulator_);
                accumulator_ = 0;
            }
        }

        void BitsWriter::WriteByte(byte symbol) {
            if (bits_count_ == 0) {
                buffer_.push_back(symbol);
            }
            else {
                accumulator_ |= symbol << bits_count_;
                buffer_.push_back(accumulator_);
                accumulator_ = symbol >> (8 - bits_count_);
            }
        }

        std::vector<byte> BitsWriter::GetResult() {
            if (bits_count_ != 0) {
                // Добавляем в буфер аккумулятор, если в нем что-то есть.
                buffer_.push_back(accumulator_);
                buffer_.front() |= static_cast<byte>(bits_count_) << LAST_BITS_START_POSITION;
            }
            return std::move(buffer_);
        }

        class BitsReader {
        public:
            BitsReader(std::vector<byte>&& buffer);
            bool ReadBit();
            byte ReadByte();
            operator bool();
        private:
            std::vector<byte> buffer_;
            std::vector<byte>::iterator it_;
            std::vector<byte>::iterator last_;
            byte accumulator_ = 0;
            byte bits_count_ = 4;
            byte last_bits_count_ = 0;
        };

        inline BitsReader::BitsReader(std::vector<byte>&& buffer) :
            buffer_(buffer),
            it_(buffer_.begin()),
            last_(buffer_.end()),
            last_bits_count_((buffer_.front() & LAST_BITS_FILLED_MASK) >> LAST_BITS_START_POSITION)
        {
            accumulator_ = *it_++ >> bits_count_;
            if (!last_bits_count_)
                last_bits_count_ = 8;
        }

        bool BitsReader::ReadBit() {
            if (bits_count_ == 0) {
                bits_count_ = 8;
                accumulator_ = *it_++;
            }
            bool result = accumulator_ & 1;
            accumulator_ = accumulator_ >> 1;
            --bits_count_;
            return result;
        }

        byte BitsReader::ReadByte() {
            byte result = 0;
            if (bits_count_ == 0) {
                result = *it_++;
            }
            else {
                result = accumulator_;
                accumulator_ = *it_++;
                result |= accumulator_ << bits_count_;
                accumulator_ = accumulator_ >> (8 - bits_count_);
            }
            return result;
        }

        inline BitsReader::operator bool() {
            return it_ < last_ || last_bits_count_ > 8 - bits_count_;
        }
    }
}
#endif // !BIT_STREAM_H_
#pragma endregion "bit_stream.h"


#pragma region "huffman_tree.h"
#ifndef HUFFMAN_TREE_H_
#define HUFFMAN_TREE_H_

#include <queue>
#include <cassert>


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
#pragma endregion "huffman_tree.h"


namespace made {

    namespace huffman {

        using made::stl::STREAM_ENCODED_MASK;
        using made::stl::BitsReader;
        using made::stl::BitsWriter;
        
        bool verbose = false;

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

            std::vector<byte> output = std::move(out.GetResult());
            // When encoded sequence longer than source, cancel encoding.
            // We still have to add a byte - signature of "not encoded"
            if (output.size() > input.size()) {
                output = std::move(input);
                output.insert(output.begin(), 0);
            }
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
                }
            }
            else {
                while (in) {
                    in.ReadBit(); // TODO : optimize with getting stream size
                    output.push_back(root->code);
                }
            }
            return output;
        }

        std::vector<byte> DecodeBytes(std::vector<byte>& input) {
            bool encoded = (input.front() & STREAM_ENCODED_MASK);
            if (encoded) {
                BitsReader in(std::move(input));
                Node* root = (new Node())->ReadTree(in);
                std::vector<byte> output = DecodeTree(root, in);
                root->Clear();
                return output;
            }
            else {
                input.erase(input.begin());
                return input;
            }
        }
    }
}
#endif // !HUFFMAN_ENCODER_H_
#pragma endregion "huffman_encoder.h"


using byte = unsigned char;
using Transformer = std::function<std::vector<byte>(std::vector<byte>&)>;

std::vector<byte> GetBytesFromStream(IInputStream& stream) {
    std::vector<byte> bytes;
    byte value;
    while (stream.Read(value)) {
        bytes.push_back(value);
    }
    return bytes;
}

void PutBytesToStream(const std::vector<byte>& bytes, IOutputStream& stream) {
    for (byte value : bytes) {
        stream.Write(value);
    }
}

void Transform(IInputStream& input, IOutputStream& output, std::vector<Transformer> transformers) {
    std::vector<byte> bytes = GetBytesFromStream(input);
    for (auto transformer : transformers) {
        bytes = transformer(bytes);
    }
    PutBytesToStream(bytes, output);
}

void Encode(IInputStream& input, IOutputStream& output) {
    Transform(input, output, { &made::huffman::EncodeBytes });
}

void Decode(IInputStream& input, IOutputStream& output) {
    Transform(input, output, { &made::huffman::DecodeBytes });
}
