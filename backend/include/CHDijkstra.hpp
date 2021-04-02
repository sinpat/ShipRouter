#pragma once

#include <Graph.hpp>

class CHDijkstra
{
public:
    CHDijkstra(const Graph& graph) noexcept;

    DijkstraPath findShortestPath(NodeId source, NodeId target) noexcept;
};