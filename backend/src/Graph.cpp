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
      snap_selled_(grid.size(), false)
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

    first_index_of_ = std::move(grid.first_index_of_);
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

    // TODO: This needs to work perfecly
    const auto raw_m = static_cast<std::int64_t>(round((theta + PI / 2) * n_rows_ / PI - 0.5));
    const auto m = (raw_m + n_rows_) % n_rows_;

    const auto m_theta = PI * (m + 0.5) / n_rows_;
    const auto m_phi = static_cast<size_t>(round(2 * PI * sin(m_theta) / d_phi_));

    const auto raw_n = static_cast<std::int64_t>(round((phi + PI) * m_phi / (2 * PI)));
    const auto n = (raw_n + m_phi) % m_phi;

    return std::pair{m, n};
}


auto Graph::gridToId(std::size_t m, std::size_t n) const noexcept
    -> NodeId
{
    return first_index_of_[m] + n;
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
    if(m == 0) {
        auto range = utils::range(first_index_of_[1]);
        return std::vector(std::begin(range),
                           std::end(range));
    }

    const auto theta = PI * (m - 0.5) / n_rows_;
    auto n_columns = static_cast<size_t>(round(2 * PI * sin(theta) / d_phi_));

    auto first_n = (n + n_columns - 1) % n_columns;
    auto second_n = (n + 1) % n_columns;


    return std::vector{
        gridToId(m - 1, n),
        gridToId(m - 1, first_n),
        gridToId(m - 1, second_n)};
}

auto Graph::getUpperGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    if(m == n_rows_ - 1) {
        auto range = utils::range(first_index_of_[1]);
        return std::vector(std::begin(range),
                           std::end(range));
    }

    const auto theta = PI * (m + 1.5) / n_rows_;
    auto n_columns = static_cast<size_t>(round(2 * PI * sin(theta) / d_phi_));


    auto first_n = (n + n_columns - 1) % n_columns;
    auto second_n = (n + 1) % n_columns;

    return std::vector{
        gridToId(m + 1, n),
        gridToId(m + 1, first_n),
        gridToId(m + 1, second_n)};
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
        auto candidate = workstack.back();
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
        for(auto neig : getNeigboursOf(best_before_insert)) {
            candidates.emplace(neig);
        }
        const auto best_after_insert = candidates.top();

        if(best_before_insert == best_after_insert) {
            break;
        }
    }

    fmt::print("initial: {} vs snapped: {}\n", candidate, candidates.top());

    return candidates.top();
}
