#include <random>
#include <cassert>
#include <iostream>
#include <vector>
#include "memcheck_crt.h"
#include "splay_tree.h"

using namespace made::stl;
using val_t = size_t;

namespace {
    std::default_random_engine generator;
    int max_gen = 10000;
    std::uniform_int_distribution<val_t> distribution(0, max_gen);
    int generate(int size) {
        return distribution(generator)*size / max_gen;
    }
}

int main() {
    ENABLE_CRT;
    int n = 500;
    std::vector<val_t> init_arr;
    std::vector<val_t> arr;
    for (int i = 0; i < n; ++i) {
        init_arr.push_back(i);
    }
    int count = 0;
    SplayTree<val_t, std::greater<>> tree;

    while (init_arr.size()) {
        bool command = generate(10) < 7;
        if (command) {
            int pos = generate(init_arr.size());
            val_t val = init_arr[pos];
            arr.push_back(val);
            std::sort(arr.begin(), arr.end(), std::greater<>());
            std::vector<val_t>::iterator it = std::find(arr.begin(), arr.end(), val);
            int index = it - arr.begin();


            //assert(index == tree.InsertSplay(val));
            assert(index == tree.Insert(val));


            std::cout << val <<" " << index << " " << arr.size() << std::endl;
            init_arr.erase(init_arr.begin() + pos);
            count++;
        }
        else if (arr.size()) {
            int pos = generate(arr.size());
            val_t val = arr[pos];
            arr.erase(arr.begin() + pos);
            //tree.DeletePos(pos);
            tree.Delete(tree.FindKOrder(pos));
        }
    }
    std::cout << std::endl << count;
}