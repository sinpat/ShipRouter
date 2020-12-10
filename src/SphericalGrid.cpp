#include <Constants.hpp>
#include <LatLng.hpp>
#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <Vector3D.hpp>
#include <algorithm>
#include <execution>

SphericalGrid::SphericalGrid(std::size_t number_of_nodes) noexcept
    : a_(4 * PI / number_of_nodes),
      n_rows_(static_cast<size_t>(round(PI / sqrt(a_)))),
      d_phi_(a_ / (PI / n_rows_)),
      first_index_of_(n_rows_, -1)
{
    size_t counter = 0;
    for(size_t m = 0; m < n_rows_; m++) {
        first_index_of_[m] = counter;
        auto theta = PI * (m + 0.5) / n_rows_;
        // auto m_phi = calc_m_phi(theta, d_phi_);
        auto n_cols = nCols(m);
        for(size_t n = 0; n < n_cols; n++) {
            auto phi = 2 * PI * n / n_cols;
            lats_.emplace_back(Latitude<Radian>{theta}.toDegree() - 90);
            lngs_.emplace_back(Longitude<Radian>{phi}.toDegree() - 180);
            counter++;
        }
    }
    first_index_of_.emplace_back(counter); // add last dummy entry
}

auto SphericalGrid::sphericalToGrid(Latitude<Radian> theta, Longitude<Radian> phi) const
    -> std::pair<size_t, size_t>
{
    auto m_phi = round(2 * PI * sin(theta) / d_phi_);
    auto m = static_cast<size_t>(floor(theta * n_rows_ - PI - 0.5));
    auto n = static_cast<size_t>(floor(phi * m_phi) / (2 * PI));
    return std::pair{m, n};
}


auto SphericalGrid::size() const noexcept
    -> std::size_t
{
    return lats_.size();
}

auto SphericalGrid::gridToID(size_t m, size_t n) const -> size_t
{
    return first_index_of_[m] + n;
}

auto SphericalGrid::idToGrid(size_t id) const -> std::pair<size_t, size_t>
{
    auto m_iter = std::upper_bound(
                      first_index_of_.begin(),
                      first_index_of_.end(),
                      id)
        - 1;
    auto m = std::distance(first_index_of_.begin(), m_iter);
    auto n = id - *m_iter;
    return std::pair{m, n};
}


auto SphericalGrid::getRowNeighbours(size_t m, size_t n) const noexcept
    -> std::vector<std::pair<std::size_t, std::size_t>>
{
    const auto n_cols_in_this = nCols(m);
    return std::vector<std::pair<std::size_t, std::size_t>>{
        std::pair<std::size_t, std::size_t>{m, (n + n_cols_in_this - 1) % n_cols_in_this},
        std::pair<std::size_t, std::size_t>{m, (n + 1) % n_cols_in_this}};
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

auto SphericalGrid::nCols(size_t row_idx) const
    -> std::size_t
{
    auto theta = PI * (row_idx + 0.5) / n_rows_;
    return static_cast<size_t>(round(2 * PI * sin(theta) / d_phi_));
}
