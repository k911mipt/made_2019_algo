/*
11_1. Цикл минимальной длины
Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.
Ввод: v: кол-во вершин (макс. 50000), n: кол-во ребер (макс. 200000), n пар реберных вершин.
Вывод: одно целое число равное длине минимального цикла. Если цикла нет, то вывести -1.
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "graph.h"




int main() {
    std::ifstream input_stream("bridges.in");
    std::cin.rdbuf(input_stream.rdbuf());
#if !defined(_MSC_VER)
    std::ofstream output_stream("bridges.out");
    std::cout.rdbuf(output_stream.rdbuf());
#endif

    size_t n_vertexes = 0;
    size_t n_edges = 0;
    std::cin >> n_vertexes >> n_edges;

    Graph G(n_vertexes);
    for (size_t i = 1; i <= n_edges; ++i) {
        size_t v1, v2;
        std::cin >> v1 >> v2;
        G.AddEdge(i, v1 - 1, v2 - 1);
    }

    std::vector<size_t> bridges = G.SearchForBridges();

    std::cout << bridges.size() << std::endl;
    for (size_t bridge : bridges) {
        std::cout << bridge << " ";
    }
    std::cout << std::endl;;
    return EXIT_SUCCESS;
}