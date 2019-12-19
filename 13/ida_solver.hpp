#pragma once
#pragma once
#ifndef IDA_SOLVER_HPP_
#define IDA_SOLVER_HPP_

#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cassert>

#include "common.h"
#include "position.hpp"

namespace IDA {

}

class IDASolver {
public:
    static std::pair<bool, std::vector<char>> SolvePuzzle(const std::vector<uint32>& start) {
        //std::unordered_map<Position, Position> parents;
        //if (!IsSolvable(start.data()) || !BFS(start, parents)) {
        //    return std::make_pair(false, std::vector<char>());
        //}
        //return std::make_pair(true, GetPath(start, parents));
        return std::pair<bool, std::vector<char>>(false, {});
    }
};

struct node {
    char pz[16];
    char pz_r[16];
};

int solutionFound = 0, abortIDA = 0;
char solution[85];

int CurrentSearchLength = 0;

//newidx[blank][dir] gives the new INTERNAL position index of the blank when moving in direction dir
//dir: 0 left, 1 right, 2 up, 3 down
char newidx[16][4] = {
    { -1,  1, -1,  7},{  0,  2, -1,  6},{  1,  3, -1,  5},{  2, -1, -1,  4},
    {  5, -1,  3, 11},{  6,  4,  2, 10},{  7,  5,  1,  9},{ -1,  6,  0,  8},
    { -1,  9,  7, 15},{  8, 10,  6, 14},{  9, 11,  5, 13},{ 10, -1,  4, 12},
    { 13, -1, 11, -1},{ 14, 12, 10, -1},{ 15, 13,  9, -1},{ -1, 14,  8, -1}
};
char manhattdist[16][16];

void treesearchMANHATTAN_(node n, char blnkpos, char lastdir, int togo) {
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (solutionFound || abortIDA) return;

    if (togo == 0) {
        solution[togo] = blnkpos;

        if (solution[84] == 0) { // Ort für Lösungslänge
            //QueryPerformanceCounter((LARGE_INTEGER*)&timeStop);
            //printSolutionPieces(Form1->puz, CurrentSearchLength, (puztype)(Form1->Type->ItemIndex));
            solution[84] = CurrentSearchLength;
            //if (Form1->SolNum->ItemIndex == 0) { //only one solution to compute
            solutionFound = 1;
            return;
            //}
        }
        else {
            if (solution[84] < CurrentSearchLength) {
                //QueryPerformanceCounter((LARGE_INTEGER*)&timeStop);
                solutionFound = 1;
                return;
            }
            //printSolutionPieces(Form1->puz, CurrentSearchLength, (puztype)(Form1->Type->ItemIndex));
            //Application->ProcessMessages();
        }

    }
    else {
        for (char dir = 0; dir < 4; dir++) {
            if (lastdir == -1 || (dir + lastdir) % 4 != 1) {
                int newblankpos = newidx[blnkpos][dir];
                if (newblankpos == -1)
                    continue;

                n.pz[blnkpos] = n.pz[newblankpos];
                n.pz[newblankpos] = 0;

                int mandi = 0;
                for (int i = 0; i < 16; i++) {
                    if (i != newblankpos)
                        mandi += manhattdist[i][n.pz[i]];
                }
                if (mandi < togo) {
                    nodecount++;
                    if (nodecount % 1000000 == 0) {
                        Application->ProcessMessages();
                    }
                    solution[togo] = blnkpos;
                    treesearchMANHATTAN_(n, newblankpos, dir, togo - 1);
                }
                n.pz[newblankpos] = n.pz[blnkpos];
                n.pz[blnkpos] = 0;

            }
        }
    }
}



//void treesearchWD(node n, char blnkpos, unsigned short wdidx, unsigned short wdidx_r, char lastdir, int togo) {
//    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    if (solutionFound || abortIDA) return;
//
//    //	char pd[16];
//    //	for (int i=0;i<16;i++) pd[i]=n.pz[i];
//
//    if (togo == 0) {
//        solution[togo] = blnkpos;
//
//        if (solution[84] == 0) { // Ort für Lösungslänge
//            QueryPerformanceCounter((LARGE_INTEGER*)&timeStop);
//            printSolutionPieces(Form1->puz, CurrentSearchLength, (puztype)(Form1->Type->ItemIndex));
//            solution[84] = CurrentSearchLength;
//            if (Form1->SolNum->ItemIndex == 0) { //only one solution to compute
//                solutionFound = 1;
//                return;
//            }
//        }
//        else {
//            if (solution[84] < CurrentSearchLength) {
//                QueryPerformanceCounter((LARGE_INTEGER*)&timeStop);
//                solutionFound = 1;
//                return;
//            }
//            printSolutionPieces(Form1->puz, CurrentSearchLength, (puztype)(Form1->Type->ItemIndex));
//            Application->ProcessMessages();
//        }
//
//    }
//    else {
//        char blnkpos_r = mirr[blnkpos];
//        for (char dir = 0; dir < 4; dir++) {
//            if (lastdir != -1 && (dir + lastdir) % 4 == 1) continue;    //so 10% schneller
//            int pos = newidx[blnkpos][dir];
//            if (pos == -1)continue;
//            int pos_r = mirr[pos];
//            unsigned short wdidxnew = wdidx, wdidx_rnew = wdidx_r;
//
//            switch (dir) {
//            case 0: //move blank left
//                wdidx_rnew = wdmoveup[wdidx_r][n.pz_r[pos_r] / 4]; break;
//            case 1: //move blank right
//                wdidx_rnew = wdmovedown[wdidx_r][n.pz_r[pos_r] / 4]; break;
//            case 2: //move blank up
//                wdidxnew = wdmoveup[wdidx][n.pz[pos] / 4]; break;
//            case 3: //move blank down
//                wdidxnew = wdmovedown[wdidx][n.pz[pos] / 4]; break;
//            }
//            if ((prunwd[wdidxnew] + prunwd[wdidx_rnew] <= togo - 1) /*&& (lastdir==-1 ||(dir+lastdir)%4!=1)*/) {
//
//                n.pz[blnkpos] = n.pz[pos]; n.pz[pos] = 0;
//                n.pz_r[blnkpos_r] = n.pz_r[pos_r]; n.pz_r[pos_r] = 0;
//
//                nodecount++;
//                if (nodecount % 1000000 == 0) {
//                    Application->ProcessMessages();
//                }
//                solution[togo] = blnkpos;
//                treesearchWD(n, pos, wdidxnew, wdidx_rnew, dir, togo - 1);
//
//                n.pz[pos] = n.pz[blnkpos]; n.pz[blnkpos] = 0;
//                n.pz_r[pos_r] = n.pz_r[blnkpos_r]; n.pz_r[blnkpos_r] = 0;
//
//            }
//        }
//    }
//}

#endif // !IDA_SOLVER_HPP_