#pragma once

#include <CHGraph.hpp>
#include <Graph.hpp>

class GraphContractor
{
public:
    GraphContractor(Graph g);
    CHGraph contract() const;

private:
    std::vector<NodeId> independentSet() const;

private:
    Graph g;
    std::vector<int64_t> level;
    std::int64_t numNodes;
};