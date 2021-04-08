#pragma once

#include <Graph.hpp>
#include <queue>

using Direction = uint;

static constexpr auto FORWARD = 0;
static constexpr auto BACKWARD = 1;

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
    // construct the path from source to target over best_node
    DijkstraPath unfoldPath(NodeId source, NodeId target) const noexcept;
    // path from `current` until `until` in the given `direction`. Does not include the `current` node
    Path from(NodeId current, NodeId until, Direction direction) const noexcept;
    void reset() noexcept;

private:
    const Graph& graph_;

    CHDijkstraQueue q_;
    std::vector<Distance> forward_dists_;
    std::vector<Distance> backward_dists_;
    std::vector<EdgeId> forward_previous_edges_;
    std::vector<EdgeId> backward_previous_edges_;
    // helper array for previous edges that allows easy access for both directions
    std::array<std::vector<EdgeId>*, 2> previous_edges_ = {&forward_previous_edges_, &backward_previous_edges_};

    // all nodes whose dists and previous' have been set
    std::vector<NodeId> touched_;
    // holds the NodeId and COMBINED distance of the best node
    std::pair<NodeId, Distance> best_node_;
};