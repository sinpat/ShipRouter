#pragma once

#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <nonstd/span.hpp>

class Graph
{
public:
    Graph(SphericalGrid&& grid);

    auto idToLat(NodeId id) const noexcept
        -> Latitude<Degree>;

    auto idToLng(NodeId id) const noexcept
        -> Longitude<Degree>;

    auto idToM(NodeId id) const noexcept
        -> std::size_t;

    auto idToN(NodeId id) const noexcept
        -> std::size_t;

    auto isValidId(NodeId id) const noexcept
        -> bool;

    auto getNeigboursOf(NodeId node) const noexcept
        -> nonstd::span<const NodeId>;

    auto getNeigboursOf(NodeId node) noexcept
        -> nonstd::span<NodeId>;

    auto getDistanceBetween(NodeId from, NodeId to) const noexcept
        -> Distance;

    auto size() const noexcept
        -> std::size_t;

private:
    std::vector<Latitude<Degree>> lats_;
    std::vector<Longitude<Degree>> lngs_;
    std::vector<std::size_t> ns_;
    std::vector<std::size_t> ms_;

    std::vector<NodeId> neigbours_;
    std::vector<size_t> offset_;
};
