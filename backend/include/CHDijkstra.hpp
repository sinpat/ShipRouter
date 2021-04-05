#pragma once

#include <Graph.hpp>
#include <queue>

using Direction = uint;

struct QNode
{
    QNode(NodeId node_in, Distance dist_in, Direction dir_in)
        : node(node_in),
          dist(dist_in),
          dir(dir_in) {}

    NodeId node;
    Distance dist;
    Direction dir; // forward or backward
};

struct CHDijkstraQueueComparer
{
    auto operator()(const QNode& lhs,
                    const QNode& rhs) const noexcept
        -> bool
    {
        return lhs.dist > rhs.dist;
    }
};

using CHDijkstraQueue = std::priority_queue<QNode,
                                            std::vector<QNode>,
                                            CHDijkstraQueueComparer>;

class CHDijkstra
{
public:
    CHDijkstra(const Graph& graph) noexcept;

    DijkstraPath findShortestPath(NodeId source, NodeId target) noexcept;

private:
    std::pair<Path, Distance> unfoldPath(NodeId node);
    void reset() noexcept;

private:
    const Graph& graph_;

    CHDijkstraQueue q_;
    std::vector<Distance> forward_dists_;
    std::vector<Distance> backward_dists_;
    std::vector<NodeId> forward_previous_;
    std::vector<NodeId> backward_previous_;
    std::vector<NodeId> touched_;
};