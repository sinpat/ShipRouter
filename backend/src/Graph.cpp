#include <Graph.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <nonstd/span.hpp>
#include <queue>


Graph::Graph(SphericalGrid&& grid)
    : offset_(grid.size() + 1, 0),
      n_rows_(grid.n_rows_),
      d_phi_(grid.d_phi_)
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

    auto current = ms_[0];
    auto counter = 0;

    first_index_of_.emplace_back(0);
    for(auto row : ms_) {
        if(row != current) {
            first_index_of_.emplace_back(counter);
        }
        counter++;
    }
    first_index_of_.emplace_back(counter);
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
    const auto start_offset = offset_[node];
    const auto end_offset = offset_[node + 1];
    const auto* start = &neigbours_[start_offset];
    const auto* end = &neigbours_[end_offset];

    return nonstd::span{start, end};
}

auto Graph::distanceBetween(NodeId from, NodeId to) const noexcept
    -> Distance
{
    const auto raw_distance = ::distanceBetween(lats_[from],
                                                lngs_[from],
                                                lats_[to],
                                                lngs_[to]);

    const auto raw_distance_cm = std::round(raw_distance * 100);

    return static_cast<Distance>(raw_distance_cm);
}

auto Graph::sphericalToGrid(Latitude<Radian> theta,
                            Longitude<Radian> phi) const noexcept
    -> std::pair<size_t, size_t>
{
    const auto m = static_cast<size_t>(std::round((theta + PI / 4) * n_rows_ / PI - 0.5));
    const auto m_phi = static_cast<size_t>(std::round(2 * PI * sin(m) / d_phi_));
    const auto n = static_cast<size_t>(std::round((phi + PI / 2) * m_phi / (2 * PI)));

    return std::pair{m, n};
}


auto Graph::gridToId(std::size_t m, std::size_t n) const noexcept
    -> NodeId
{
    return first_index_of_[m] + n;
}

auto Graph::snapToGridNode(Latitude<Degree> lat,
                           Longitude<Degree> lng) const noexcept
    -> NodeId
{
    const auto [m, n] = sphericalToGrid(lat.toRadian(), lng.toRadian());
    const auto source_id = gridToId(m, n);

    std::priority_queue candidates(
        [&](auto id1, auto id2) {
            return ::distanceBetween(lat, lng, lats_[id1], lngs_[id1])
                > ::distanceBetween(lat, lng, lats_[id2], lngs_[id2]);
        },
        std::vector{source_id});

    while(true) {
        const auto best_before_insert = candidates.top();
        for(auto neig : getNeigboursOf(best_before_insert)) {
            candidates.emplace(neig);
        }
        const auto best_after_insert = candidates.top();

        if(best_before_insert == best_after_insert) {
            break;
        }
    }

    return candidates.top();
}
