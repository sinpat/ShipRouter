#pragma once
#include <OSMNode.hpp>

struct Edge
{
    size_t source;
    size_t destination;
};

class Graph
{
public:
    Graph(const std::vector<OSMNode> nodes, const std::vector<Edge> edges)
        : nodes_(nodes), edges_(edges)
    {
        std::vector<size_t> offset(nodes.size() + 1, 0);
        for(const auto edge : edges) {
            offset[edge.source + 1]++;
        }
        for(auto i = 1; i < nodes.size(); i++) {
            offset[i] = offset[i + 1];
        }
        offset_ = offset;
    }

private:
    std::vector<OSMNode> nodes_;
    std::vector<Edge> edges_;
    std::vector<size_t> offset_;
};