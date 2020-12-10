#pragma once

#include <SphericalGrid.hpp>
#include <functional>
#include <optional>
#include <queue>
#include <string_view>
#include <vector>

using Distance = std::uint64_t;
using NodeId = std::uint64_t;
using Path = std::vector<NodeId>;


constexpr static inline auto UNREACHABLE = std::numeric_limits<Distance>::max();


struct DijkstraQueueComparer
{
    auto operator()(const std::pair<NodeId, Distance>& lhs,
                    const std::pair<NodeId, Distance>& rhs) const noexcept
        -> bool
    {
        return lhs.second > rhs.second;
    }
};

using DijkstraQueue = std::priority_queue<std::pair<NodeId, Distance>,
                                          std::vector<std::pair<NodeId, Distance>>,
                                          DijkstraQueueComparer>;




class Dijkstra
{
public:
    Dijkstra(const SphericalGrid& graph) noexcept;
    Dijkstra() = delete;
    Dijkstra(Dijkstra&&) = default;
    Dijkstra(const Dijkstra&) = default;
    auto operator=(const Dijkstra&) -> Dijkstra& = delete;
    auto operator=(Dijkstra&&) -> Dijkstra& = delete;

    [[nodiscard]] auto findRoute(NodeId source, NodeId target) noexcept
        -> std::optional<Path>;

    [[nodiscard]] auto findDistance(NodeId source, NodeId target) noexcept
        -> Distance;

private:
    [[nodiscard]] auto getDistanceTo(NodeId n) const noexcept
        -> Distance;

    auto setDistanceTo(NodeId n, Distance distance) noexcept
        -> void;

    [[nodiscard]] auto computeDistance(NodeId source, NodeId target) noexcept
        -> Distance;

    [[nodiscard]] auto extractShortestPath(NodeId source, NodeId target) const noexcept
        -> std::optional<Path>;

    auto unSettle(NodeId n)
        -> void;

    auto settle(NodeId n) noexcept
        -> void;

    [[nodiscard]] auto isSettled(NodeId n)
        -> bool;

    auto reset() noexcept
        -> void;

private:
    const SphericalGrid& graph_;
    std::vector<Distance> distances_;
    std::vector<bool> settled_;
    std::vector<NodeId> touched_;
    DijkstraQueue pq_;
    std::optional<NodeId> last_source_;
};
