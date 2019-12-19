/*
14_1. MST. Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
Вариант 1. С помощью алгоритма Прима.
*/

#include <iostream>
#include <fstream>
#include "graph.h"

using uint = uint32_t;

int main() {
#if defined(_MSC_VER)
    std::ifstream input_stream("kruskal.in");
    std::cin.rdbuf(input_stream.rdbuf());
#endif
//#if !defined(_MSC_VER)
//    std::ofstream output_stream("kruskal.out");
//    std::cout.rdbuf(output_stream.rdbuf());
//#endif

    uint n_vertexes = 0;
    uint n_edges = 0;
    std::cin >> n_vertexes >> n_edges;

    Graph G(n_vertexes);
    for (uint i = 1; i <= n_edges; ++i) {
        uint v1, v2, weight;
        std::cin >> v1 >> v2 >> weight;
        G.AddEdge(v1 - 1, v2 - 1, weight);
    }
    std::cout << G.MST_Prim() << std::endl;
    return EXIT_SUCCESS;
}