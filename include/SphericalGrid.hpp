#pragma once

#include <LatLng.hpp>
#include <Polygon.hpp>
#include <Utils.hpp>
#include <cmath>
#include <vector>

class SphericalGrid
{
public:
    SphericalGrid(std::size_t number_of_nodes) noexcept;

    auto getLats() const noexcept
        -> const std::vector<Lat<DegreeTag>>&;
    auto getLngs() const noexcept
        -> const std::vector<Lng<DegreeTag>>&;

    auto getLats() noexcept
        -> std::vector<Lat<DegreeTag>>&;
    auto getLngs() noexcept
        -> std::vector<Lng<DegreeTag>>&;

    auto sphericalToGrid(double theta, double phi) const
        -> std::pair<std::size_t, std::size_t>;

    auto gridToID(size_t m, size_t n) -> size_t;

    auto IDToGrid(size_t ID) -> std::pair<size_t, size_t>;

    auto indexIsWater(std::size_t idx) const noexcept
        -> bool;

    auto indexIsLand(std::size_t idx) const noexcept
        -> bool;

    auto filter(const std::vector<Polygon>& polygons) noexcept
        -> void;

private:
    double a_;
    double m_theta_;
    double d_phi_;
    std::vector<size_t> first_index_of_;
    std::vector<Lat<DegreeTag>> lats_;
    std::vector<Lng<DegreeTag>> lngs_;
    std::vector<bool> is_water_;
};
