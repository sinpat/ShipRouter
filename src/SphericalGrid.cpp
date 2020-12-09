#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <Vector3D.hpp>

SphericalGrid::SphericalGrid() noexcept
{
    std::vector<std::pair<Lat, Lng>> nodes;
    auto a = 4 * M_PI / 500;
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

auto filterLandNodes(const std::vector<Polygon>& polygons,
                     SphericalGrid&& grid) noexcept
    -> SphericalGrid
{
    std::vector<std::size_t> indices;

    auto range = utils::range(grid.lats_.size());

    std::copy_if(std::begin(range),
                 std::end(range),
                 std::back_inserter(indices),
                 [&](auto idx) {
                     auto lat = grid.lats_[idx];
                     auto lng = grid.lngs_[idx];
                     return std::none_of(std::begin(polygons),
                                         std::end(polygons),
                                         [&](const Polygon& polygon) {
                                             return polygon.pointInPolygon(lat, lng);
                                         });
                 });

    std::vector<Lat> new_lats;
    std::vector<Lng> new_lngs;

    std::transform(std::begin(indices),
                   std::end(indices),
                   std::back_inserter(new_lats),
                   [&](auto idx) {
                       return grid.lats_[idx];
                   });

    std::transform(std::begin(indices),
                   std::end(indices),
                   std::back_inserter(new_lngs),
                   [&](auto idx) {
                       return grid.lngs_[idx];
                   });

    grid.lats_ = std::move(new_lats);
    grid.lngs_ = std::move(new_lngs);

    return std::move(grid);
}
