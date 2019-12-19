#include <unordered_set>
#include <unordered_map>
#include <set>
#include <vector>

using std::unordered_set;
using std::unordered_map;
using std::set;
using std::vector;

using uint = uint32_t;

struct Edge {
    uint v = ~0;
    uint weight = ~0;
    Edge(const uint _v, const uint _weight) : v(_v), weight(_weight) {}
    bool operator<(const Edge& rhs) const& {
        return weight < rhs.weight || (weight == rhs.weight && v < rhs.v);
    }
};

class Graph {
private:
    vector<vector<Edge>> vertexes_;
public:
    explicit Graph(uint v_count)
        : vertexes_(v_count)
    {
    }

    void AddEdge(uint v1, uint v2, uint weight) {
        if (v1 == v2) {
            return; // self-loops cannot be bridges
        }
        vertexes_[v1].emplace_back(v2, weight);
        vertexes_[v2].emplace_back(v1, weight);
    }
    uint MST_Prim() {
        uint result = 0;
        set<Edge> edges; // add edges from every new MST node here to choose best at each iteration
        unordered_set<uint> used; // vertexes already picked in MST
        std::unordered_map<uint, uint> weights_to; // key - vertex num, value - minimal weight *to* vertex
        edges.emplace(0, 0);
        while (!edges.empty()) {
            auto e_from = *edges.begin();
            edges.erase(edges.begin());
            used.insert(e_from.v);
            result += e_from.weight;

            auto& vertex_edges = vertexes_[e_from.v];
            for (uint i = 0; i < vertex_edges.size(); ++i) {
                while (i != vertex_edges.size() && used.find(vertex_edges[i].v) != used.end()) {
                    // Get rid of all used edges to not iterate through them again
                    vertex_edges[i] = vertex_edges.back();
                    vertex_edges.pop_back();
                }
                if (i == vertex_edges.size()) {
                    continue;
                }
                const auto& e_to = vertex_edges[i];
                const auto& existing_edge = weights_to.find(e_to.v);
                if (existing_edge == weights_to.end()) {
                    // candidate edge to be picked
                    edges.insert(e_to);
                    weights_to[e_to.v] = e_to.weight;
                }
                else if (existing_edge->second > e_to.weight) {
                    // replace previous candidate edge with a better one
                    edges.erase({ existing_edge->first, existing_edge->second });
                    edges.insert(e_to);
                    weights_to[e_to.v] = e_to.weight;
                }
            }
        }
        return result;
    }
};