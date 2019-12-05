#include <vector>
#include <functional>

#include "Huffman.h"
#include "huffman_encoder.h"

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