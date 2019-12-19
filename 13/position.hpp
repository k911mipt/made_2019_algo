#pragma once
#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <iostream>
#include <vector>
#include <functional>
#include <array>

#include "common.h"

namespace {
    inline constexpr uint64_t pack_position(const uint32 start[16]) {
        uint64_t position = 0;
        uint8 index = 0;
        uint8 shift = 0;
        uint64_t zero_pos = 0;
        uint64_t tag = 0;
        for (uint8 i = 0; i < 4; ++i) {
            for (uint8 j = 0; j < 4; ++j) {
                tag = static_cast<uint64_t>(start[index++]);
                shift += tag ? 1 : 0;
                position += tag ? tag << (shift * 4) : 0;
                zero_pos = tag ? zero_pos : shift;
            }
        }
        position += zero_pos;
        return position;
    }

    inline void print_position(uint64_t position) {
        std::cout << position << std::endl;
        uint64_t zero_pos = position & 0xFull;
        uint8 index = 0;
        uint8 shift = 0;
        uint64_t digit;
        for (uint8 i = 0; i < 4; ++i) {
            for (uint8 j = 0; j < 4; ++j) {
                shift += index == zero_pos ? 0 : 1;
                digit = index++ == zero_pos ? 0 : (position >> (shift * 4)) & 0xFull;
                std::cout << static_cast<int>(digit) << "\t";
            }
            std::cout << std::endl;
        }
    }

    inline uint32 Manhattan(uint64 position) {
        uint32 result = 0;
        uint64 zero_pos = position & 0xFull;
        uint8 shift = 0;
        uint8 digit;
        for (uint8 i = 0; i < 16; ++i) {
            if (i != zero_pos) {
                ++shift;
                digit = ((position >> (shift * 4)) & 0xFull) - 1; // decrease value by 1 to compare with raw index

                uint8 x_at = i & 0b11u;
                uint8 y_at = i >> 2;

                uint8 x_to = digit & 0b11u;
                uint8 y_to = digit >> 2;

                result += abs(x_at - x_to) + abs(y_at - y_to);
            }
        }
        return result;
    }

    bool IsSolvable(const uint32 puzzle[16]) {
        uint32 parity = 0;
        bool odd_row;
        const uint32 size = 16;
        for (int i = 0; i < size; i++)
            if (puzzle[i] != 0)
                for (int j = i + 1; j < size; j++)
                    parity += (puzzle[i] > puzzle[j] && puzzle[j] != 0) ? 1 : 0;
            else
                odd_row = (i / 4) % 2;
        return parity % 2 != odd_row;
    }
}

enum Direction {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
};
static constexpr std::array<Direction, 4> Directions{ Up, Down, Left, Right };


class Position {
private:
    uint64 position = 0;
    uint32 cost = 0;
    static constexpr uint32 finish_arr[16]{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 };
    static constexpr uint64 finish = pack_position(finish_arr);
public:
    Position(const uint64 _position) noexcept
        : position(_position)
    {
        CountCost();
    }
    Position(const uint32 start[16]) noexcept : Position(pack_position(start)) {}
    Position(const std::vector<uint32>(pos)) noexcept : Position(pos.data()) {}
    Position(const std::initializer_list<uint32>& pos) noexcept : Position(std::vector<uint32>(pos)) {}
    Position() noexcept : Position(finish) {}

    void CountCost() {
        cost = Heuristic(position);
    }


    void Print() const noexcept {
        std::cout << cost << " ";
        print_position(position);
    }

    uint64 Move(Direction direction) const { return movers[direction](position); }
    bool AllowedMove(Direction direction) const { return move_allowers[direction](position); }

    bool operator<(const Position& rhs) const& { return cost < rhs.cost; }
    bool operator>(const Position& rhs) const& { return cost > rhs.cost; }
    bool operator==(const Position& rhs) const& { return position == rhs.position; }
    bool operator!=(const Position& rhs) const& { return !(*this == rhs); }

    bool IsFinish() const {
        return position == finish;
    }

    size_t Raw() const {
        return position;
    }
private:
    /*
        Possible zero positions:

        CanNotMoveRight, mask = xx11
          |
        0000 0001 0010 0011 <-- CanNotMoveDown, mask = 11xx
        0100 0101 0110 0111
        1000 1001 1010 1011
        1100 1101 1110 1111 <-- CanNotMoveUp, mask = 11xx
                        |
        CanNotMoveLeft, mask = xx11
    */
    static bool AllowedUp(uint64 position) noexcept { return (position & 0b1100ull) != 0b1100ull; }
    static bool AllowedDown(uint64 position) noexcept { return (position & 0b1100ull) != 0ull; }
    static bool AllowedLeft(uint64 position) noexcept { return (position & 0b0011ull) != 0b0011ull; }
    static bool AllowedRight(uint64 position) noexcept { return (position & 0b0011ull) != 0ull; }

    static uint64 MoveUp(uint64 position) {
        uint64 result = position + 4;
        uint64 zero_pos = (result & 0xFull);
        uint64 moving_digit = (result >> ((zero_pos) * 4)) & 0xFull;
        uint64 moving_three = (result >> ((zero_pos - 4) * 4)) & 0xFFF0ull;
        uint64 moved_four = (moving_three + moving_digit) << ((zero_pos - 3) * 4);
        uint64 clear_mask = ~(0xFFFFull << ((zero_pos - 3) * 4));
        return (result & clear_mask) | moved_four;
    }
    static uint64 MoveDown(uint64 position) {
        uint64 result = position - 4;
        uint64 zero_pos = (result & 0xFull);
        uint64 moving_digit = ((result >> ((zero_pos + 1) * 4)) & 0x000Full) << 12;
        uint64 moving_three = (result >> ((zero_pos + 2) * 4)) & 0xFFFull;
        uint64 moved_four = (moving_three + moving_digit) << ((zero_pos + 1) * 4);
        uint64 clear_mask = ~(0xFFFFull << ((zero_pos + 1) * 4));
        return (result & clear_mask) | moved_four;
    }
    static uint64 MoveLeft(uint64 position) { return position + 1; }
    static uint64 MoveRight(uint64 position) { return position - 1; }
private:
    static std::function<bool(uint64)> move_allowers[4];
    static std::function<uint64(uint64)> movers[4];
    static std::function<uint32(uint64)> Heuristic;
};

namespace std {
    template <>
    struct hash<Position> {
        size_t operator()(const Position& x) const {
            return x.Raw();
        }
    };
}
#endif // !POSITION_HPP_