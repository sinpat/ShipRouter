#include <CHDijkstra.hpp>
#include <fmt/ranges.h>

CHDijkstra::CHDijkstra(const Graph& graph) noexcept
    : graph_(graph),
      forward_dists_(graph_.size(), UNREACHABLE),
      backward_dists_(graph_.size(), UNREACHABLE),
      forward_previous_edges_(graph_.size(), NON_EXISTENT),
      backward_previous_edges_(graph_.size(), NON_EXISTENT)
{}

DijkstraPath CHDijkstra::findRoute(NodeId source, NodeId target) noexcept
{
    reset(); // TODO: remove this and try to optimize
    std::array done = {false, false}; // indicates whether we are done with forward resp. backward search
    q_.emplace(source, 0, FORWARD);
    q_.emplace(target, 0, BACKWARD);
    forward_dists_[source] = 0;
    backward_dists_[target] = 0;
    touched_.emplace_back(source);
    touched_.emplace_back(target);
    uint q_pops = 0;

    while(!q_.empty()) {
        QNode q_node = q_.top();
        NodeId cur_node = q_node.node;
        Direction direction = q_node.dir;
        q_.pop();
        q_pops++;

        // check if we have to continue exploring in this direction
        if(done[direction]) {
            continue;
        } else if(q_node.dist > best_node_.second) {
            if(done[direction xor 1]) {
                break;
            }
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
            if(dist_with_edge < (*dists[direction])[target]) {
                (*dists[direction])[target] = dist_with_edge;
                (*previous_edges_[direction])[target] = edge_id;
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
    return unfoldPath(source, target, q_pops);
}

DijkstraPath CHDijkstra::unfoldPath(NodeId source, NodeId target, uint pops) const noexcept
{
    if(best_node_.first == NON_EXISTENT) {
        return std::nullopt;
    }
    auto [node, dist] = best_node_;

    // unfold forward path
    std::vector<NodeId> path = from(node, source, FORWARD);
    path.emplace_back(node);
    // unfold backward path
    std::vector<NodeId> backward_path = from(node, target, BACKWARD);
    std::reverse(backward_path.begin(), backward_path.end());
    path.insert(path.end(), backward_path.begin(), backward_path.end());
    return std::tuple{
        path,
        dist,
        pops};
}

Path CHDijkstra::from(NodeId current, NodeId until, Direction direction) const noexcept
{
    if(current == until) {
        return std::vector<NodeId>{};
    }
    EdgeId prev_edge_id = (*previous_edges_[direction])[current];
    Path wrapped = graph_.unwrapEdge(prev_edge_id, current); // all nodes represented by prev_edge
    NodeId next_current = wrapped[0];
    Path path = from(next_current, until, direction); // path before prev_edge
    path.insert(path.end(), wrapped.begin(), wrapped.end());
    return path;
};


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