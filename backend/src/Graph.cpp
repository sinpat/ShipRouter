#include <Graph.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <nonstd/span.hpp>


Graph::Graph(SphericalGrid&& grid)
    : offset_(grid.size() + 1, 0)
{
    for(auto id : utils::range(grid.size())) {
        if(grid.indexIsLand(id)) {
            continue;
        }

        auto neigs = grid.getNeighbours(id);

        std::sort(std::begin(neigs),
                  std::end(neigs));
        auto remove_iter =
            std::unique(std::begin(neigs),
                        std::end(neigs));

        neigs.erase(remove_iter, std::end(neigs));

        neigbours_ = concat(std::move(neigbours_),
                            std::move(neigs));

        offset_[id + 1] = neigbours_.size();

        auto [m, n] = grid.idToGrid(id);
        ns_.emplace_back(n);
        ms_.emplace_back(m);

        auto [lat, lng] = grid.idToLatLng(id);
        lats_.emplace_back(lat);
        lngs_.emplace_back(lng);
    }

    //insert dummy at the end
    neigbours_.emplace_back(std::numeric_limits<NodeId>::max());
}

auto Graph::idToLat(NodeId id) const noexcept
    -> Latitude<Degree>
{
    return lats_[id];
}

auto Graph::idToLng(NodeId id) const noexcept
    -> Longitude<Degree>
{
    return lngs_[id];
}

auto Graph::idToM(NodeId id) const noexcept
    -> std::size_t
{
    return ms_[id];
}

auto Graph::idToN(NodeId id) const noexcept
    -> std::size_t
{
    return ns_[id];
}

auto Graph::isValidId(NodeId id) const noexcept
    -> bool
{
    return id < size();
}

auto Graph::size() const noexcept
    -> std::size_t
{
    return lats_.size();
}

auto Graph::getNeigboursOf(NodeId node) const noexcept
    -> nonstd::span<const NodeId>
{
    auto start_offset = offset_[node];
    auto end_offset = offset_[node + 1];
    const auto* start = &neigbours_[start_offset];
    const auto* end = &neigbours_[end_offset];

    return nonstd::span{start, end};
}

auto Graph::getNeigboursOf(NodeId node) noexcept
    -> nonstd::span<NodeId>
{
    auto start_offset = offset_[node];
    auto end_offset = offset_[node + 1];
    auto* start = &neigbours_[start_offset];
    auto* end = &neigbours_[end_offset];

    return nonstd::span(start, end);
}

auto Graph::getDistanceBetween(NodeId from, NodeId to) const noexcept
    -> Distance
{
    auto raw_distance = distanceBetween(lats_[from],
                                        lngs_[from],
                                        lats_[to],
                                        lngs_[to]);

    auto raw_distance_cm = std::round(raw_distance * 100);

    return static_cast<Distance>(raw_distance_cm);
}
