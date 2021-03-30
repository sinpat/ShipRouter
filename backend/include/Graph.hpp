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

    auto relaxEdges(NodeId node) const noexcept
        -> nonstd::span<const Edge>;

    auto relaxEdgesWithIds(NodeId node) const noexcept
        -> std::pair<nonstd::span<const Edge>, nonstd::span<const EdgeId>>;

    auto size() const noexcept
        -> std::size_t;

    auto snapToGridNode(Latitude<Degree> lat, Longitude<Degree> lng) const noexcept
        -> NodeId;

    auto gridToId(std::size_t m, std::size_t n) const noexcept
        -> NodeId;

    auto isLandNode(NodeId node) const noexcept
        -> bool;

    void contract() noexcept;

private:
    auto getSnapNodeCandidate(Latitude<Degree> lat,
                              Longitude<Degree> lng) const noexcept
        -> NodeId;

    auto getUpperGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
        -> std::vector<NodeId>;

    auto getLowerGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
        -> std::vector<NodeId>;

    auto getRowGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
        -> std::vector<NodeId>;

    auto getGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
        -> std::vector<NodeId>;

    // for contraction
    void contractionStep() noexcept;
    std::vector<NodeId> independentSet() const;
    void insertEdges(NodeId source, std::vector<Edge> edges);

private:
    std::vector<std::size_t> ns_;
    std::vector<std::size_t> ms_;

    std::vector<Edge> edges_;
    std::vector<size_t> offset_;

    mutable std::vector<bool> snap_settled_;
    const SphericalGrid grid_;

    // for ch-graph
    std::vector<Level> levels;
    /** flags for edges that have been replaced by a shortcut */
    std::vector<bool> contracted_edges;

    Level current_level = 0;
    bool fully_contracted = false;
};
