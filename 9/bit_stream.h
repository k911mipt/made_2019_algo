#pragma once
#ifndef BIT_STREAM_H_
#define BIT_STREAM_H_

#include <vector>

namespace made {

    namespace stl {
        using byte = unsigned char;

        class BitsWriter {
        public:
            void WriteBit(bool bit);
            void WriteByte(byte byte);
            std::vector<unsigned char> GetResult();
        private:
            std::vector<unsigned char> buffer_;
            unsigned char accumulator_ = 0;
            int bits_count_ = 0;
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
            }
            buffer_.push_back(static_cast<unsigned char>(bits_count_));
            return std::move(buffer_);
        }

        class BitsReader {
        public:
            BitsReader(std::vector<byte>&& buffer);
            bool ReadBit();
            byte ReadByte();
            operator bool();
            void SetBuffer(std::vector<byte>&& buffer);
        private:
            std::vector<byte> buffer_;
            std::vector<byte>::iterator it_;
            std::vector<byte>::iterator last_;
            byte accumulator_ = 0;
            int bits_count_ = 0;
            int last_bits_count_ = 0;
            int bytes_count_ = 0;
        };

        inline BitsReader::BitsReader(std::vector<byte>&& buffer) :
            buffer_(buffer),
            it_(buffer_.begin()),
            last_(buffer_.end() - 1),
            last_bits_count_(static_cast<int>(buffer_.back()))
        {}

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
            //if (it_ < last_)
            //    return true;
            //return (last_bits_count_ > 8 - bits_count_);
            return it_ < last_ ? true : last_bits_count_ > 8 - bits_count_;
        }

        inline void BitsReader::SetBuffer(std::vector<byte>&& buffer) {
            buffer_ = buffer;
            it_ = buffer_.begin();
            last_bits_count_ = static_cast<int>(buffer_.back());
        }
    }
}
#endif // !BIT_STREAM_H_