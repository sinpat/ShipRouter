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
        -> const std::vector<Lat>&;
    auto getLngs() const noexcept
        -> const std::vector<Lng>&;

    auto getLats() noexcept
        -> std::vector<Lat>&;
    auto getLngs() noexcept
        -> std::vector<Lng>&;

    auto filter(const std::vector<Polygon>& polygons) noexcept
        -> void;

private:
    std::vector<Lat> lats_;
    std::vector<Lng> lngs_;
    std::vector<bool> is_water_;
};
