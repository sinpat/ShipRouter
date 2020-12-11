#include <Constants.hpp>
#include <LatLng.hpp>
#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <Vector3D.hpp>
#include <algorithm>
#include <execution>
#include <queue>

SphericalGrid::SphericalGrid(std::size_t number_of_nodes) noexcept
    : a_(4 * PI / number_of_nodes),
      n_rows_(static_cast<size_t>(round(PI / sqrt(a_)))),
      d_phi_(a_ / (PI / n_rows_)),
      first_index_of_(n_rows_ + 1, -1)
{
    size_t counter = 0;
    for(size_t m = 0; m < n_rows_; m++) {
        first_index_of_[m] = counter;
        auto theta = calcTheta(m);
        for(size_t n = 0; n < nCols(m); n++) {
            auto phi = calcPhi(m, n);
            lats_.emplace_back(Latitude<Radian>{theta}.toDegree() - 90);
            lngs_.emplace_back(Longitude<Radian>{phi}.toDegree() - 180);
            counter++;
        }
    }
    first_index_of_[n_rows_] = (counter); // add last dummy entry
}

auto SphericalGrid::sphericalToGrid(Latitude<Radian> theta, Longitude<Radian> phi) const noexcept
    -> std::pair<size_t, size_t>
{
    const auto m_phi = nCols(theta);
    const auto m = static_cast<size_t>(floor(theta * n_rows_ - PI - 0.5));
    const auto n = static_cast<size_t>(floor(phi * m_phi) / (2 * PI));
    return std::pair{m, n};
}


auto SphericalGrid::idToLatLng(NodeId n) const noexcept
    -> std::pair<Latitude<Degree>, Longitude<Degree>>
{
    return std::pair{lats_[n], lngs_[n]};
}

auto SphericalGrid::gridToSpherical(size_t m, size_t n) const
    -> std::pair<Latitude<Degree>, Longitude<Degree>>
{
    const auto theta = calcTheta(m);
    const auto phi = calcPhi(m, n);
    return std::pair{
        Latitude<Radian>{theta}.toDegree(),
        Longitude<Radian>{phi}.toDegree()};
}

auto SphericalGrid::size() const noexcept
    -> std::size_t
{
    return lats_.size();
}

auto SphericalGrid::gridToID(size_t m, size_t n) const noexcept
    -> size_t
{
    return first_index_of_[m] + n;
}

auto SphericalGrid::idToGrid(size_t id) const noexcept
    -> std::pair<size_t, size_t>
{
    const auto m_iter = std::upper_bound(
                            first_index_of_.begin(),
                            first_index_of_.end(),
                            id)
        - 1;
    const auto m = std::distance(first_index_of_.begin(), m_iter);
    const auto n = id - *m_iter;
    return std::pair{m, n};
}

auto SphericalGrid::getRowNeighbours(size_t m, size_t n) const noexcept
    -> std::vector<std::pair<std::size_t, std::size_t>>
{
    const auto n_cols_in_this = nCols(m);
    return std::vector{std::pair{m, (n + n_cols_in_this - 1) % n_cols_in_this},
                       std::pair{m, (n + 1) % n_cols_in_this}};
}

auto SphericalGrid::getLowerNeighbours(size_t m, size_t n) const noexcept
    -> std::vector<std::pair<std::size_t, std::size_t>>
{
    std::vector<std::pair<size_t, size_t>> neighbours;
    if(m == 0) {
        for(size_t i = 0; i < first_index_of_[1]; i++) {
            neighbours.emplace_back(0, i);
        }
        return neighbours;
    }
    const auto m_idx_lower = (m + n_rows_ - 1) % n_rows_;
    const auto n_cols_in_this = nCols(m);
    const auto n_cols_in_lower = nCols(m_idx_lower);
    const auto lower_row_ratio = static_cast<double>(n_cols_in_lower) / n_cols_in_this;
    for(int i = floor((static_cast<int>(n) - 1) * lower_row_ratio); i <= ceil((n + 1) * lower_row_ratio); i++) {
        neighbours.emplace_back(m_idx_lower, (i + n_cols_in_lower) % n_cols_in_lower);
    }
    return neighbours;
}

auto SphericalGrid::getUpperNeighbours(size_t m, size_t n) const noexcept
    -> std::vector<std::pair<std::size_t, std::size_t>>
{
    std::vector<std::pair<size_t, size_t>> neighbours;
    if(m == n_rows_ - 1) {
        for(size_t i = 0; i < first_index_of_[m + 1] - first_index_of_[m]; i++) {
            neighbours.emplace_back(m, i);
        }
        return neighbours;
    }
    const auto m_idx_upper = (m + 1) % n_rows_;
    const auto n_cols_in_this = nCols(m);
    const auto n_cols_in_upper = nCols(m_idx_upper);
    const auto upper_row_ratio = static_cast<double>(n_cols_in_upper) / n_cols_in_this;
    for(int i = floor((static_cast<int>(n) - 1) * upper_row_ratio); i <= ceil((n + 1) * upper_row_ratio); i++) {
        neighbours.emplace_back(m_idx_upper, (i + n_cols_in_upper) % n_cols_in_upper);
    }
    return neighbours;
}

auto SphericalGrid::getNeighbours(size_t m, size_t n) const noexcept
    -> std::vector<size_t>
{
    auto grid_neighbours = concat(
        getRowNeighbours(m, n),
        getUpperNeighbours(m, n),
        getLowerNeighbours(m, n));

    std::vector<size_t> id_neighbours;

    // map (m,n) to IDs
    std::transform(
        grid_neighbours.begin(),
        grid_neighbours.end(),
        std::back_inserter(id_neighbours),
        [&](auto neighbour) {
            return gridToID(neighbour.first, neighbour.second);
        });

    // filter out land nodes
    id_neighbours.erase(
        std::remove_if(
            id_neighbours.begin(),
            id_neighbours.end(),
            [&](auto id) {
                return indexIsLand(id);
            }),
        std::end(id_neighbours));
    return id_neighbours;
}

auto SphericalGrid::getNeighbours(std::size_t id) const noexcept
    -> std::vector<size_t>
{

    auto [m, n] = idToGrid(id);
    return getNeighbours(m, n);
}

auto SphericalGrid::distanceBetween(NodeId from, NodeId to) const noexcept
    -> Distance
{
    auto from_lat = lats_[from];
    auto to_lat = lats_[to];

    auto from_lng = lngs_[from];
    auto to_lng = lngs_[to];

    return ::distanceBetween(from_lat, from_lng, to_lat, to_lng);
}

auto SphericalGrid::snapToNode(Latitude<Degree> lat, Longitude<Degree> lng) const
    -> size_t
{
    const auto [m, n] = sphericalToGrid(lat.toRadian(), lng.toRadian());
    const auto source_id = gridToID(m, n);

    std::priority_queue candidates(
        [&](const size_t id1, const size_t id2) {
            return distanceBetween(source_id, id1) > distanceBetween(source_id, id2);
        },
        std::vector{source_id});

    while(true) {
        const auto best_before_insert = candidates.top();
        for(auto neig : getNeighbours(best_before_insert)) {
            candidates.emplace(neig);
        }
        const auto best_after_insert = candidates.top();
        if(best_before_insert == best_after_insert) {
            break;
        }
    }
    return candidates.top();
}

auto SphericalGrid::getLats() const noexcept
    -> const std::vector<Latitude<Degree>>&
{
    return lats_;
}

auto SphericalGrid::getLngs() const noexcept
    -> const std::vector<Longitude<Degree>>&
{
    return lngs_;
}

auto SphericalGrid::getLats() noexcept
    -> std::vector<Latitude<Degree>>&
{
    return lats_;
}

auto SphericalGrid::getLngs() noexcept
    -> std::vector<Longitude<Degree>>&
{
    return lngs_;
}

auto SphericalGrid::isValidId(NodeId from) const noexcept
    -> bool
{
    return from < lngs_.size();
}

auto SphericalGrid::indexIsWater(std::size_t idx) const noexcept
    -> bool
{
    return is_water_[idx];
}

auto SphericalGrid::indexIsLand(std::size_t idx) const noexcept
    -> bool
{
    return !is_water_[idx];
}

auto SphericalGrid::filter(const std::vector<Polygon>& polygons) noexcept
    -> void
{
    auto range = utils::range(lats_.size());
    is_water_.resize(range.size());

    std::transform(std::execution::par,
                   std::cbegin(range),
                   std::cend(range),
                   std::begin(is_water_),
                   [&](auto idx) {
                       const auto lat = lats_[idx];
                       const auto lng = lngs_[idx];
                       return std::none_of(std::cbegin(polygons),
                                           std::cend(polygons),
                                           [&](const Polygon& polygon) {
                                               return polygon.pointInPolygon(lat, lng);
                                           });
                   });
}

auto SphericalGrid::nCols(size_t m) const
    -> std::size_t
{
    auto theta = calcTheta(m);
    return nCols(theta);
}

auto SphericalGrid::nCols(double theta) const -> std::size_t
{
    return static_cast<size_t>(round(2 * PI * sin(theta) / d_phi_));
}

auto SphericalGrid::calcTheta(size_t m) const -> double
{
    return PI * (m + 0.5) / n_rows_;
}

auto SphericalGrid::calcPhi(size_t m, size_t n) const -> double
{
    return 2 * PI * n / nCols(m);
}