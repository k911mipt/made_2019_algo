#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>

using std::unordered_set;
using std::unordered_map;
using std::vector;

struct Vertex {
    unordered_set<size_t> neighbours;
    unordered_map<size_t, unordered_set<size_t>> edges;
    size_t disc = 0;
    size_t low = 0;
    void AddNeighbour(size_t neighbour_num, size_t edge_num) {
        neighbours.insert(neighbour_num);
        edges[neighbour_num].insert(edge_num);
    }
};

struct VertexInfo {
    static const size_t no_parent = ~0;

    bool visited = false;
    size_t disc = 0;
    size_t low = 0;
    size_t parent = no_parent;

    using it_type = decltype(Vertex::neighbours.begin());
    it_type it;
    it_type it_end;
};

class Graph {
private:
    size_t size_;
    vector<Vertex> vertexes_;

public:
    explicit Graph(size_t v_count)
        : size_(v_count),
        vertexes_(v_count)
    {
    }

    void AddEdge(size_t edge_num, size_t v1, size_t v2) {
        if (v1 == v2) {
            return; // self-loops cannot be bridges
        }
        vertexes_[v1].AddNeighbour(v2, edge_num);
        vertexes_[v2].AddNeighbour(v1, edge_num);
    }

    std::vector<size_t> SearchForBridges() {
        /*
            General idea is taken from https://www.geeksforgeeks.org/bridge-in-a-graph/
        */
        std::vector<size_t> bridges;
        if (!size_) { // zero bridges for zero vertexes
            return bridges;
        }
        size_t time = 0;
        vector<VertexInfo> vinfos(size_);
        for (size_t i = 0; i < size_; ++i) {
            vinfos[i].it = vertexes_[i].neighbours.begin();
            vinfos[i].it_end = vertexes_[i].neighbours.end();
        }

        for (size_t i = 0; i < size_; i++) {
            if (!vinfos[i].visited) {
                //SeekBridgesFromVertexRecursive(i, time, vinfos, bridges);
                SeekBridgesFromVertexPlain(i, time, vinfos, bridges);
            }
        }
        std::sort(bridges.begin(), bridges.end());
        return bridges;
    }

private:
    void SeekBridgesFromVertexRecursive(size_t vstart, size_t& time, vector<VertexInfo>& vinfos, std::vector<size_t>& bridges) {
        ++time;
        auto& info = vinfos[vstart];
        // Mark the current node as visited 
        info.visited = true;
        // Initialize discovery time and low value
        info.disc = time;
        info.low = info.disc;
        auto& v = vertexes_[vstart];
        auto& v_edges = v.edges;
        for (size_t v_to : v.neighbours) { // we iterate only neighbours. We check for multiple edges later.
            auto& cur_info = vinfos[v_to];
            if (!cur_info.visited) {
                cur_info.parent = vstart;
                SeekBridgesFromVertexRecursive(v_to, time, vinfos, bridges);
                info.low = std::min(info.low, cur_info.low);
                if (cur_info.low > info.disc) {
                    if (v_edges[v_to].size() == 1) // check for multiple edges
                        bridges.push_back(*v_edges[v_to].begin());
                }
            }
            else if (v_to != info.parent) {
                info.low = std::min(info.low, cur_info.disc);
            }
        }
    }

    // Unrolled version of SeekBridgesFromVertexRecursive
    void SeekBridgesFromVertexPlain(size_t vstart, size_t& time, vector<VertexInfo>& vinfos, std::vector<size_t>& bridges) {
        size_t v_from = vstart;
        while (v_from != VertexInfo::no_parent) {
            auto& info_from = vinfos[v_from];
            if (!info_from.visited) {
                ++time;
                info_from.visited = true;
                info_from.disc = time;
                info_from.low = info_from.disc;
            }

            if (info_from.it != info_from.it_end) {
                size_t v_to = *info_from.it;
                auto& info_to = vinfos[v_to];
                // If got an unvisited vertex, step deeper. Otherwise, continue iterating
                if (!info_to.visited) {
                    info_to.parent = v_from;
                    v_from = v_to;
                }
                else if (v_to != info_from.parent) {
                    info_from.low = std::min(info_from.low, info_to.disc);
                }
                info_from.it++;
            }
            else {
                // Step upper
                if (info_from.parent == VertexInfo::no_parent) {
                    return;
                }
                auto v_parent = info_from.parent;
                auto& info_parent = vinfos[v_parent];
                info_parent.low = std::min(info_parent.low, info_from.low);
                if (info_from.low > info_parent.disc) {
                    auto& v_edges = vertexes_[v_parent].edges;
                    if (v_edges[v_from].size() == 1)
                        bridges.push_back(*v_edges[v_from].begin());
                }
                v_from = v_parent;
            }
        }
    }
};