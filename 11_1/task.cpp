/*
11_1. Цикл минимальной длины
Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.
Ввод: v: кол-во вершин (макс. 50000), n: кол-во ребер (макс. 200000), n пар реберных вершин.
Вывод: одно целое число равное длине минимального цикла. Если цикла нет, то вывести -1.
*/

#include <iostream>
#include <fstream>

#include <vector>
#include <unordered_set>
#include <queue>
#include <algorithm>

using std::unordered_set;
using std::vector;
using std::queue;

struct Vertex {
    std::unordered_set<size_t> neighbours;
    void AddNeighbour(size_t neighbour_num) {
        neighbours.insert(neighbour_num);
    }
};

class Graph {
private:
    vector<Vertex> vertexes;
public:
    explicit Graph(size_t v_count) 
        : vertexes(v_count) 
    {
    }
    void AddEdge(size_t v1, size_t v2) {
        vertexes[v1].AddNeighbour(v2);
        vertexes[v2].AddNeighbour(v1);
    }
    int SearchForMinLoop() {
        size_t cycle_dist = vertexes.size() + 1;
        for (size_t i = 0; i < vertexes.size(); ++i) {
            cycle_dist = std::min(SearchForLoopFromVertex(i), cycle_dist);
        }
        return static_cast<int>(cycle_dist > vertexes.size() ? -1 : cycle_dist);
    }
    size_t SearchForLoopFromVertex(size_t vstart) {
        size_t size = vertexes.size();
        size_t cycle_dist = size + 1;
        vector<size_t> dist(size, size); // distance to start vertex
        vector<size_t> prev(size, size); // previously visited vertex. this is needed to avoid (1,0)-(0,1) loops
        dist[vstart] = 0;
        queue<size_t> vqueue;
        queue<size_t> temp_vqueue;
        vqueue.push(vstart);
        while (!vqueue.empty()) {
            size_t cur_v = vqueue.front(); vqueue.pop();
            size_t cur_d = dist[cur_v];
            // no need to bfs more than half of vertexes. if there is a cycle, it will be found at other search iterations
            if (cur_d * 2 > cycle_dist) { 
                return cycle_dist;
            }
            for (size_t neighbour : vertexes[cur_v].neighbours) {
                // skipping vertexes from previous iteration
                if (neighbour < vstart) { 
                    continue;
                }
                if (dist[neighbour] == size) {
                    dist[neighbour] = cur_d + 1;
                    vqueue.push(neighbour);
                    prev[neighbour] = cur_v;
                }
                else if (prev[cur_v] != neighbour) {
                    return cur_d + 1 + dist[neighbour];
                }
            }
        }
        return cycle_dist;
    }
};

int main() {
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    size_t n_vertexes = 0;
    size_t n_edges = 0;
    std::cin >> n_vertexes >> n_edges;

    Graph G(n_vertexes);
    for (size_t i = 0; i < n_edges; ++i) {
        size_t v1, v2;
        std::cin >> v1 >> v2;
        G.AddEdge(v1, v2);
    }
    std::cout << G.SearchForMinLoop() << std::endl;

    return EXIT_SUCCESS;
}