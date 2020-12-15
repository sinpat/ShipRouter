#include <Graph.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <nonstd/span.hpp>
#include <queue>


Graph::Graph(SphericalGrid&& grid)
    : offset_(grid.size() + 1, 0),
      n_rows_(grid.n_rows_),
      d_phi_(grid.d_phi_),
      snap_selled_(grid.size(), false),
      grid_(std::move(grid))
{
    for(auto id : utils::range(grid.size())) {
        if(!grid.indexIsLand(id)) {
            auto neigs = grid.getNeighbours(id);

            std::sort(std::begin(neigs),
                      std::end(neigs));
            auto remove_iter =
                std::unique(std::begin(neigs),
                            std::end(neigs));

            neigs.erase(remove_iter, std::end(neigs));

            std::vector<std::pair<NodeId, Distance>> neig_dist;
            std::transform(std::begin(neigs),
                           std::end(neigs),
                           std::back_inserter(neig_dist),
                           [&](auto neig) {
                               auto [start_lat, start_lng] = grid.idToLatLng(id);
                               auto [dest_lat, dest_lng] = grid.idToLatLng(neig);
                               auto distance = ::distanceBetween(start_lat, start_lng, dest_lat, dest_lng);

                               return std::pair{neig, distance};
                           });

            neigbours_ = concat(std::move(neigbours_),
                                std::move(neig_dist));
        }

        offset_[id + 1] = neigbours_.size();

        auto [m, n] = grid.idToGrid(id);
        ns_.emplace_back(n);
        ms_.emplace_back(m);

        auto [lat, lng] = grid.idToLatLng(id);
        lats_.emplace_back(lat);
        lngs_.emplace_back(lng);
    }

    //insert dummy at the end
    neigbours_.emplace_back(std::numeric_limits<NodeId>::max(), UNREACHABLE);

    first_index_of_ = std::move(grid.first_index_of_);
}

auto Graph::idToLat(NodeId id) const noexcept
    -> Latitude<Degree>
{
    return grid_.lats_[id];
}

auto Graph::idToLng(NodeId id) const noexcept
    -> Longitude<Degree>
{
    return grid_.lngs_[id];
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
    return grid_.lats_.size();
}

auto Graph::getNeigboursOf(NodeId node) const noexcept
    -> nonstd::span<const std::pair<NodeId, Distance>>
{
    const auto start_offset = offset_[node];
    const auto end_offset = offset_[node + 1];
    const auto* start = &neigbours_[start_offset];
    const auto* end = &neigbours_[end_offset];

    return nonstd::span{start, end};
}

auto Graph::sphericalToGrid(Latitude<Radian> theta,
                            Longitude<Radian> phi) const noexcept
    -> std::pair<size_t, size_t>
{
    return grid_.sphericalToGrid(theta, phi);
}


auto Graph::gridToId(std::size_t m, std::size_t n) const noexcept
    -> NodeId
{
    return grid_.gridToID(m, n);
}

auto Graph::isLandNode(NodeId node) const noexcept
    -> bool
{
    return offset_[node] == offset_[node + 1];
}


auto Graph::getRowGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::array<NodeId, 2>
{
    const auto theta = PI * (m + 0.5) / n_rows_;
    auto n_columns = static_cast<size_t>(round(2 * PI * sin(theta) / d_phi_));

    auto first_n = (n + n_columns - 1) % n_columns;
    auto second_n = (n + 1) % n_columns;

    return std::array{gridToId(m, first_n),
                      gridToId(m, second_n)};
}

auto Graph::getLowerGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    auto on_grid = grid_.getLowerNeighbours(m, n);
    std::vector<NodeId> ids;
    std::transform(std::begin(on_grid),
                   std::end(on_grid),
                   std::back_inserter(ids),
                   [&](auto pair) {
                       auto [m, n] = pair;
                       return gridToId(m, m);
                   });

    return ids;
}

auto Graph::getUpperGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    auto on_grid = grid_.getUpperNeighbours(m, n);
    std::vector<NodeId> ids;
    std::transform(std::begin(on_grid),
                   std::end(on_grid),
                   std::back_inserter(ids),
                   [&](auto pair) {
                       auto [m, n] = pair;
                       return gridToId(m, m);
                   });

    return ids;
}

auto Graph::getGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    return concat(getUpperGridNeigboursOf(m, n),
                  getLowerGridNeigboursOf(m, n),
                  getRowGridNeigboursOf(m, n));
}

auto Graph::getSnapNodeCandidate(Latitude<Degree> lat,
                                 Longitude<Degree> lng) const noexcept
    -> NodeId
{
    const auto [m, n] = sphericalToGrid(lat.toRadian(), lng.toRadian());

    const auto id = gridToId(m, n);

    std::vector<NodeId> candidates;
    if(!isLandNode(id)) {
        candidates.emplace_back(id);
    }

    auto workstack = getGridNeigboursOf(m, n);

    while(!workstack.empty()) {
        const auto candidate = workstack.back();
        workstack.pop_back();
        snap_selled_[candidate] = true;
        snap_touched_.emplace_back(candidate);

        if(!isLandNode(candidate)) {
            candidates.emplace_back(candidate);
            continue;
        }

        if(snap_selled_[candidate]) {
            continue;
        }

        workstack = concat(std::move(workstack),
                           getGridNeigboursOf(ms_[candidate],
                                              ns_[candidate]));
    }

    for(auto touched : snap_touched_) {
        snap_selled_[touched] = false;
    }
    snap_touched_.clear();

    return *std::min_element(std::cbegin(candidates),
                             std::cend(candidates),
                             [&](auto lhs, auto rhs) {
                                 auto lhs_lat = lats_[lhs];
                                 auto lhs_lng = lngs_[lhs];

                                 auto rhs_lat = lats_[rhs];
                                 auto rhs_lng = lngs_[rhs];

                                 return ::distanceBetween(lat, lng, lhs_lat, lhs_lng)
                                     < ::distanceBetween(lat, lng, rhs_lat, rhs_lng);
                             });
}

auto Graph::snapToGridNode(Latitude<Degree> lat,
                           Longitude<Degree> lng) const noexcept
    -> NodeId
{
    auto candidate = getSnapNodeCandidate(lat, lng);

    std::priority_queue candidates(
        [&](auto id1, auto id2) {
            return ::distanceBetween(lat, lng, lats_[id1], lngs_[id1])
                > ::distanceBetween(lat, lng, lats_[id2], lngs_[id2]);
        },
        std::vector{candidate});

    while(true) {
        const auto best_before_insert = candidates.top();
        for(auto [neig, _] : getNeigboursOf(best_before_insert)) {
            candidates.emplace(neig);
        }
        const auto best_after_insert = candidates.top();

        if(best_before_insert == best_after_insert) {
            break;
        }
    }

    return candidates.top();
}
