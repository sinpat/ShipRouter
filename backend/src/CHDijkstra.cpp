#include <CHDijkstra.hpp>

CHDijkstra::CHDijkstra(const Graph& graph) noexcept
    : graph_(graph),
      forward_dists_(graph_.size(), UNREACHABLE),
      backward_dists_(graph_.size(), UNREACHABLE),
      forward_previous_(graph_.size(), NON_EXISTENT),
      backward_previous_(graph_.size(), NON_EXISTENT)
{}

DijkstraPath CHDijkstra::findShortestPath(NodeId source, NodeId target) noexcept
{
    reset(); // TODO: remove this and try to optimize
    q_.emplace(source, 0, 0);
    q_.emplace(target, 0, 1);

    while(!q_.empty()) {
        QNode q_node = q_.top();
        NodeId cur_node = q_node.node;
        q_.pop();

        auto edges = graph_.relaxEdges(cur_node); // TODO: need to be filtered by level
        for(auto edge : edges) {
            NodeId target = edge.target;
            Distance combined_dist = q_node.dist + edge.dist;
            // TODO: handle forward and backward properly
            if(combined_dist < forward_dists_[target]) {
                forward_dists_[target] = combined_dist;
                forward_previous_[target] = cur_node;
                touched_.emplace_back(target);
                q_.emplace(target, combined_dist, q_node.dir);
            }
        }
    }
    return std::nullopt;
}

std::pair<Path, Distance> CHDijkstra::unfoldPath(NodeId node)
{
    Distance dist = forward_dists_[node] + backward_dists_[node];
    // TODO: unfold
    return std::pair{
        std::vector<NodeId>{},
        dist};
}

void CHDijkstra::reset() noexcept
{
    q_ = CHDijkstraQueue{};
    for(auto id : touched_) {
        forward_dists_[id] = UNREACHABLE;
        backward_dists_[id] = UNREACHABLE;
        forward_previous_[id] = NON_EXISTENT;
        backward_previous_[id] = NON_EXISTENT;
    }
    touched_.clear();
}