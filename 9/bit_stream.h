#pragma once
#ifndef BIT_STREAM_H_
#define BIT_STREAM_H_

#include <vector>

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