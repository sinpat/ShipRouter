#include <CHDijkstra.hpp>
#include <fmt/ranges.h>

CHDijkstra::CHDijkstra(const Graph& graph) noexcept
    : graph_(graph),
      forward_dists_(graph_.size(), UNREACHABLE),
      backward_dists_(graph_.size(), UNREACHABLE),
      forward_previous_edges_(graph_.size(), NON_EXISTENT),
      backward_previous_edges_(graph_.size(), NON_EXISTENT)
{}

DijkstraPath CHDijkstra::findShortestPath(NodeId source, NodeId target) noexcept
{
    reset(); // TODO: remove this and try to optimize
    std::array done = {false, false}; // indicates whether we are done with forward resp. backward search
    q_.emplace(source, 0, FORWARD);
    q_.emplace(target, 0, BACKWARD);
    forward_dists_[source] = 0;
    backward_dists_[target] = 0;
    touched_.emplace_back(source);
    touched_.emplace_back(target);

    while(!q_.empty()) {
        QNode q_node = q_.top();
        NodeId cur_node = q_node.node;
        Direction direction = q_node.dir;
        q_.pop();

        // check if we have to continue exploring in this direction
        if(done[direction]) {
            continue;
        } else if(q_node.dist > best_node_.second) {
            done[direction] = true;
            continue;
        }

        auto edge_ids = graph_.relaxEdgeIds(cur_node); // we can use this for both searches
        for(auto edge_id : edge_ids) {
            const auto& edge = graph_.getEdge(edge_id);
            NodeId target = edge.target;
            if(graph_.getLevel(edge.source) >= graph_.getLevel(target)) {
                continue;
            }
            Distance dist_with_edge = q_node.dist + edge.dist;
            std::array dists = {&forward_dists_, &backward_dists_};
            std::array previous_edges = {&forward_previous_edges_, &backward_previous_edges_};
            if(dist_with_edge < (*dists[direction])[target]) {
                (*dists[direction])[target] = dist_with_edge;
                (*previous_edges[direction])[target] = edge_id;
                touched_.emplace_back(target);
                q_.emplace(target, dist_with_edge, direction);

                // check if we have new best
                // TODO: Is this the right place? If yes, we do not have to check both distances against unreachable
                auto other_dist = (*dists[direction xor 1])[target];
                auto combined_dist = dist_with_edge + other_dist;
                if(other_dist != UNREACHABLE and combined_dist < best_node_.second) {
                    best_node_ = std::pair{target, combined_dist};
                }
            }
        }
    }
    return unfoldPath();
}

DijkstraPath CHDijkstra::unfoldPath()
{
    if(best_node_.first == NON_EXISTENT) {
        return std::nullopt;
    }
    auto [node, dist] = best_node_;
    // TODO: unfold
    return std::pair{
        std::vector<NodeId>{},
        dist};
}

void CHDijkstra::reset() noexcept
{
    q_ = CHDijkstraQueue{};
    best_node_ = std::pair{NON_EXISTENT, UNREACHABLE};
    for(auto id : touched_) {
        forward_dists_[id] = UNREACHABLE;
        backward_dists_[id] = UNREACHABLE;
        forward_previous_edges_[id] = NON_EXISTENT;
        backward_previous_edges_[id] = NON_EXISTENT;
    }
    touched_.clear();
}