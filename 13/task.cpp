#include <iostream>
#include <fstream>
#include <vector>

#include "memcheck_crt.h"

#include "a_solver.hpp"
#include "ida_solver.hpp"

#include "puzzle15.hpp" // temporary

//std::function<uint32(uint64)> Position::Heuristic = Manhattan;

#define __A
#ifdef __A
inline void verbose(std::string s) {}
#else
#include <string>
inline void verbose(std::string s) {
    for (auto& a : s)
        std::cout << a;
}
#endif

using std::vector;
int main() {
    verbose("abc");
    ENABLE_CRT;
    


    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    vector<uint32> start;
    for (uint8 i = 0; i < 16; ++i) {
        uint32 val;
        std::cin >> val;
        start.push_back(val);
    }

    const auto result = ASolver::SolvePuzzle(start);
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

    if (!IsSolvable(start.data())) {
        std::cout << -1 << std::endl;
    }
    else {
        for (int i = 0; i < MAXP; i++)
            tr[i] = i / 4, tc[i] = i % 4;

        int sr, sc;
        int k = 0;
        for (int i = 0; i < MAXN; i++)
            for (int j = 0; j < MAXN; j++)
            {
                tile[i][j] = start[k++];
                if (tile[i][j]) tile[i][j]--;
                else          tile[i][j] = 15, sr = i, sc = j;
            }
        int cost = heuristic(tile);
        upper = std::min(MAXSTEP, cost + 1); //set a lowerbound for your DFS limit
        while (!pass)
        {
            IDASolver(0, sr, sc, cost, -1);
            upper = std::min(upper + 5, MAXSTEP); //iterative depthening
        }
    }
    return EXIT_SUCCESS;
}