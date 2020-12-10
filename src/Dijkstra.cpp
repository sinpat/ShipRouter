#include <Dijkstra.hpp>
#include <SphericalGrid.hpp>
#include <functional>
#include <numeric>
#include <optional>
#include <queue>
#include <string_view>
#include <vector>

Dijkstra::Dijkstra(const SphericalGrid& graph) noexcept
    : graph_(graph),
      distances_(graph_.size(), UNREACHABLE),
      settled_(graph_.size(), false),
      pq_(DijkstraQueueComparer{}) {}


auto Dijkstra::findRoute(NodeId source, NodeId target) noexcept
    -> std::optional<Path>
{
    [[maybe_unused]] auto _ = computeDistance(source, target);
    return extractShortestPath(source, target);
}

auto Dijkstra::findDistance(NodeId source, NodeId target) noexcept
    -> Distance
{
    return computeDistance(source, target);
}

auto Dijkstra::getDistanceTo(NodeId n) const noexcept
    -> Distance
{
    return distances_[n];
}

auto Dijkstra::setDistanceTo(NodeId n, Distance distance) noexcept
    -> void
{
    distances_[n] = distance;
}

auto Dijkstra::extractShortestPath(NodeId source, NodeId target) const noexcept
    -> std::optional<Path>
{
    //check if a path exists
    if(UNREACHABLE == getDistanceTo(target)) {
        return std::nullopt;
    }

    Path path{target};

    while(path.front() != source) {
        const auto& last_inserted = path.front();
        auto neigbours = graph_.getNeighbours(last_inserted);

        auto min_iter = std::min_element(
            std::begin(neigbours),
            std::end(neigbours),
            [&](const auto& lhs, const auto& rhs) {
                return getDistanceTo(lhs) < getDistanceTo(rhs);
            });

        //this can never happen
        if(std::end(neigbours) == min_iter) {
            return std::nullopt;
        }

        auto min_neig = *min_iter;

        //this can also never happen
        if(getDistanceTo(min_neig) == UNREACHABLE) {
            return std::nullopt;
        }

        path.insert(std::begin(path), min_neig);
    }

    return path;
}

auto Dijkstra::reset() noexcept
    -> void
{
    for(auto n : touched_) {
        unSettle(n);
        setDistanceTo(n, UNREACHABLE);
    }
    touched_.clear();
    pq_ = DijkstraQueue{DijkstraQueueComparer{}};
}

auto Dijkstra::unSettle(NodeId n)
    -> void
{
    settled_[n] = false;
}

auto Dijkstra::settle(NodeId n) noexcept
    -> void
{
    settled_[n] = true;
}

auto Dijkstra::isSettled(NodeId n)
    -> bool
{
    return settled_[n];
}

auto Dijkstra::computeDistance(NodeId source, NodeId target) noexcept
    -> Distance
{
    if(source == last_source_
       && isSettled(target)) {
        return getDistanceTo(target);
    }

    if(source != last_source_) {
        last_source_ = source;
        reset();
        pq_.emplace(source, 0l);
        setDistanceTo(source, 0);
        touched_.emplace_back(source);
    }

    while(!pq_.empty()) {
        auto [current_node, current_dist] = pq_.top();

        settle(current_node);

        if(current_node == target) {
            return current_dist;
        }

        //pop after the return, otherwise we loose a value
        //when reusing the pq
        pq_.pop();

        auto neigbours = graph_.getNeighbours(current_node);

        for(auto&& neig : neigbours) {
            auto neig_dist = getDistanceTo(neig);
            auto new_dist = current_dist + 1;

            if(UNREACHABLE != current_dist and neig_dist > new_dist) {
                touched_.emplace_back(neig);
                setDistanceTo(neig, new_dist);
                pq_.emplace(neig, new_dist);
            }
        }
    }

    return getDistanceTo(target);
}
