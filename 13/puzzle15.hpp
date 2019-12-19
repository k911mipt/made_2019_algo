#pragma once
#ifndef PUZZLE_15_H_
#define PUZZLE_15_H_

#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>
#include <cassert>
#include <unordered_map>
#include <queue>
#include <algorithm>

#include "common.h"
#include "position.hpp"



const int MAXN = 4, MAXSTEP = 80, MAXP = 15; //Maximum length of map size, Maximum Step, Maximum number of puzzle grids
int tile[MAXN][MAXN];
const int dr[4] = { 1, 0, -1, 0 }, dc[4] = { 0, 1, 0, -1 }, opp[4] = { 2, 3, 0, 1 }; //delta row, delta column, opposite direction of (dr[i], dc[i])
//For example, (dr[0], dr[0]) is down, which means its opposite is up , then opp[0] is 2. (which is up)
int tr[MAXP], tc[MAXP], upper; //tile row recording, tile column recording
char rec[MAXSTEP]; //recording answer
//const char name[4] = { 'D', 'R', 'U', 'L' };
const char name[4] = { 'U', 'L', 'D', 'R' };
bool pass;

void initialization()
{
    for (int i = 0; i < MAXP; i++)
        tr[i] = i / 4, tc[i] = i % 4;
}

int heuristic(int tile[][MAXN]) //we use Manhattan Distance as heuristic function
{
    int ret = 0;
    for (int i = 0; i < MAXN; i++)
        for (int j = 0; j < MAXN; j++)
            if (tile[i][j] < 15)
                ret += abs(i - tr[tile[i][j]]) + abs(j - tc[tile[i][j]]);
    return ret;
    return ret ? ret / 3 + 1 : 0;
}
bool valid(int r, int c) //check if a position is out of boundary
{
    return r >= 0 && r < MAXN && c >= 0 && c < MAXN;
}
void IDASolver(int depth, int r, int c, int est, int pre) //Depth of this function, row of the blank, column of the blank, actual cost, previous opposite direction (avoiding stupid moves)
{
    if (pass) return;
    est = std::max(0, est);
    if (est == 0 && heuristic(tile) == 0) //you find an answer. output it
    {
        std::cout << depth << std::endl;
        for (int i = 0; i < depth; i++)
            printf("%c", rec[i]);
        printf("\n");
        pass = true;
        return;
    }

    for (int i = 0; i < 4; i++) //check all possible direction
        if (i != pre) //don't go for a stupid move. for example, go left after you go right
        {
            int nr = r + dr[i], nc = c + dc[i], oCost, nCost, bak;

            if (valid(nr, nc))
            {
                //update new heuristic cost, new cost = actual cost + heuristic cost
                bak = tile[nr][nc];
                oCost = abs(nr - tr[bak]) + abs(nc - tc[bak]);
                nCost = abs(r - tr[bak]) + abs(c - tc[bak]);
                //heuristic(tile);

                if (depth + est + nCost - oCost + 1 <= upper) //if actual cost + heuristic cost <= your upperbound, then you can keep going in this branch
                {
                    tile[r][c] = bak, tile[nr][nc] = 15;
                    rec[depth] = name[i];
                    IDASolver(depth + 1, nr, nc, est + nCost - oCost, opp[i]);
                    tile[r][c] = 15, tile[nr][nc] = bak;
                    if (pass) return;
                }
            }
        }
}



#endif // !PUZZLE_15_H_