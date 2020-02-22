#include <iostream>
#include <fstream>
#include <vector>


#pragma region "memcheck_crt.h"
#ifndef MEMCHECK_CRT_
#define MEMCHECK_CRT_

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define ENABLE_CRT\
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);\
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);\
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#else
#define ENABLE_CRT ;
#endif

#endif // !MEMCHECK_CRT_
#pragma endregion "memcheck_crt.h"



#pragma region "a_solver.hpp"
#ifndef A_SOLVER_HPP_
#define A_SOLVER_HPP_

#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cassert>


#pragma region "common.h"
#ifndef COMMON_H_
#define COMMON_H_

using uint8 = uint_fast8_t;
using uint32 = uint_fast32_t;
using uint64 = uint_fast64_t;
#endif // !COMMON_H_
#pragma endregion "common.h"


#pragma region "position.hpp"
#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <functional>
#include <array>


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

    bool isSolvable(const uint32 puzzle[16]) {
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

namespace {

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

    std::function<bool(uint64)> Position::move_allowers[4] = { &Position::AllowedUp , &Position::AllowedDown , &Position::AllowedLeft, &Position::AllowedRight };
    std::function<uint64(uint64)> Position::movers[4] = { &Position::MoveUp , &Position::MoveDown , &Position::MoveLeft, &Position::MoveRight };

    std::function<uint32(uint64)> Position::Heuristic = Manhattan;
}

namespace std {
    template <>
    struct hash<Position> {
        size_t operator()(const Position& x) const {
            return x.Raw();
        }
    };
}
#endif // !POSITION_HPP_
#pragma endregion "position.hpp"


class A15Solver {
public:
    static std::pair<bool, std::vector<char>> SolvePuzzle(const std::vector<uint32>& start) {
        std::unordered_map<Position, Position> parents;
        if (!isSolvable(start.data()) || !BFS(start, parents)) {
            return std::make_pair(false, std::vector<char>());
        }
        return std::make_pair(true, GetPath(start, parents));
    }
private:
    static bool BFS(const Position& start, std::unordered_map<Position, Position>& parents) {
        std::priority_queue<Position, std::vector<Position>, std::greater<>> queue{};
        queue.push(start);
        while (!queue.empty()) {
            Position cur = queue.top();
            queue.pop();
            if (cur.IsFinish())
                return true;
            for (auto direction : Directions) {
                if (cur.AllowedMove(direction)) {
                    Position next = cur.Move(direction);
                    if (!parents.count(next)) {
                        parents.emplace(next, cur);
                        queue.push(next);
                    }
                }
            }
        }
        return false;
    }

    static std::vector<char> GetPath(const Position& start, const std::unordered_map<Position, Position>& parents) {
        std::vector<char> result;
        Position current;
        while (current != start) {
            const Position parent = parents.at(current);
            result.push_back(GetMoveSymbol(parent, current));
            current = parent;
        }
        // Reverse result, cause we builded it opposite
        std::reverse(result.begin(), result.end());
        return result;
    }

    static char GetMoveSymbol(const Position& from, const Position& to) {
        const uint8 zero_from = from.Raw() & 0xFull;
        const uint8 zero_to = to.Raw() & 0xFull;
        char zero_diff = (zero_to)-(zero_from);
        switch (zero_diff) {
        case 1:
            return 'L'; // Zero to right -> tile to left
        case -1:
            return 'R';
        case 4:
            return 'U';
        case -4:
            return 'D';
        default:
            assert(false);
        }
        return 0;
    }
};


#endif // !SOLVER_HPP_
#pragma endregion "a_solver.hpp"


using std::vector;
int main() {
    ENABLE_CRT;

    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    vector<uint32> start;
    for (uint8 i = 0; i < 16; ++i) {
        uint32 val;
        std::cin >> val;
        start.push_back(val);
    }

    const auto result = A15Solver::SolvePuzzle(start);
    if (result.first) {
        std::cout << result.second.size() << std::endl;
        for (const auto n_move : result.second) {
            std::cout << n_move;
        }
        std::cout << std::endl;
    }
    else {
        std::cout << -1 << std::endl;
    }
    return EXIT_SUCCESS;
}
