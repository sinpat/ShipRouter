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
      m_theta_(round(PI / sqrt(a_))),
      d_phi_(a_ / (PI / m_theta_)),
      first_index_of_(m_theta_, -1)
{
    size_t counter = 0;
    for(size_t m = 0; m < m_theta_; m++) {
        first_index_of_[m] = counter;
        auto theta = PI * (m + 0.5) / m_theta_;
        auto m_phi = round(2 * PI * sin(theta) / d_phi_);
        for(size_t n = 0; n < m_phi; n++) {
            auto phi = 2 * PI * n / m_phi;
            lats_.emplace_back(Lat<RadianTag>{theta}.toDegree() - 90);
            lngs_.emplace_back(Lng<RadianTag>{phi}.toDegree() - 180);
            counter++;
        }
    }
}

auto SphericalGrid::sphericalToGrid(double theta, double phi) const
  -> std::pair<size_t, size_t>
{
    auto m_phi = round(2 * PI * sin(theta) / d_phi_);
    auto m = static_cast<size_t>(floor(theta * m_theta_ - PI - 0.5));
    auto n = static_cast<size_t>(floor(phi * m_phi) / (2 * PI));
    return std::pair{m, n};
}

auto SphericalGrid::gridToID(size_t m, size_t n) -> size_t
{
    return first_index_of_[m] + n;
}

auto SphericalGrid::IDToGrid(size_t ID) -> std::pair<size_t, size_t>
{
    auto m_iter = std::upper_bound(
                      first_index_of_.begin(),
                      first_index_of_.end(),
                      ID)
        - 1;
    auto m = std::distance(first_index_of_.begin(), m_iter);
    auto n = ID - *m_iter;
    return std::pair{m, n};
}

auto SphericalGrid::getLats() const noexcept
  -> const std::vector<Lat<DegreeTag>>&
{
    return lats_;
}
auto SphericalGrid::getLngs() const noexcept
  -> const std::vector<Lng<DegreeTag>>&
{
    return lngs_;
}

auto SphericalGrid::getLats() noexcept
  -> std::vector<Lat<DegreeTag>>&
{
    return lats_;
}
auto SphericalGrid::getLngs() noexcept
  -> std::vector<Lng<DegreeTag>>&
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
