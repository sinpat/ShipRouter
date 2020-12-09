#include <LatLng.hpp>
#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <Vector3D.hpp>

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
            lats_.emplace_back(Lat{theta}.toDegree());
            lngs_.emplace_back(Lng{phi}.toDegree());
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

auto SphericalGrid::filter(const std::vector<Polygon>& polygons) noexcept
    -> void
{
    auto range = utils::range(lats_.size());
    is_water_.reserve(range.size());

    std::transform(std::begin(range),
                   std::end(range),
                   std::back_inserter(is_water_),
                   [&](auto idx) {
                       auto lat = lats_[idx];
                       auto lng = lngs_[idx];
                       return std::none_of(std::begin(polygons),
                                           std::end(polygons),
                                           [&](const Polygon& polygon) {
                                               return polygon.pointInPolygon(lat, lng);
                                           });
                   });
}
