#include <LatLng.hpp>
#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <Vector3D.hpp>
#include <algorithm>
#include <execution>

SphericalGrid::SphericalGrid(std::size_t number_of_nodes) noexcept
{
    std::vector<std::pair<Lat, Lng>> nodes;
    auto a = 4 * M_PI / number_of_nodes;
    auto d = sqrt(a);
    auto m_theta = round(M_PI / d);
    auto d_theta = M_PI / m_theta;
    auto d_phi = a / d_theta;
    for(size_t m = 0; m < m_theta; m++) {
        auto theta = M_PI * (m + 0.5) / m_theta;
        auto m_phi = round(2 * M_PI * sin(theta) / d_phi);
        for(size_t n = 0; n < m_phi; n++) {
            auto phi = 2 * M_PI * n / m_phi;
            lats_.emplace_back(Lat{theta}.toDegree() - 90);
            lngs_.emplace_back(Lng{phi}.toDegree() - 180);
        }
    }
}

auto SphericalGrid::getLats() const noexcept
    -> const std::vector<Lat>&
{
    return lats_;
}
auto SphericalGrid::getLngs() const noexcept
    -> const std::vector<Lng>&
{
    return lngs_;
}

auto SphericalGrid::getLats() noexcept
    -> std::vector<Lat>&
{
    return lats_;
}
auto SphericalGrid::getLngs() noexcept
    -> std::vector<Lng>&
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
