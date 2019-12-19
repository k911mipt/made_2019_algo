#pragma once
#pragma once
#ifndef A_SOLVER_HPP_
#define A_SOLVER_HPP_

#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cassert>

#include "common.h"
#include "position.hpp"

class ASolver {
public:
    static std::pair<bool, std::vector<char>> SolvePuzzle(const std::vector<uint32>& start) {
        std::unordered_map<Position, Position> parents;
        if (!IsSolvable(start.data()) || !BFS(start, parents)) {
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