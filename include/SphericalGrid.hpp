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
        -> const std::vector<Latitude<Degree>>&;
    auto getLngs() const noexcept
        -> const std::vector<Longitude<Degree>>&;

    auto getLats() noexcept
        -> std::vector<Latitude<Degree>>&;
    auto getLngs() noexcept
        -> std::vector<Longitude<Degree>>&;

    auto sphericalToGrid(Latitude<Radian> theta, Longitude<Radian> phi) const
        -> std::pair<std::size_t, std::size_t>;

    auto get_row_neighbours(size_t m, size_t n)
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto get_upper_neighbours(size_t m, size_t n)
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto get_lower_neighbours(size_t m, size_t n)
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto get_neighbours(size_t m, size_t n)
        -> std::vector<size_t>;

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
    size_t n_rows_;
    double d_phi_;
    std::vector<size_t> first_index_of_;
    std::vector<Latitude<Degree>> lats_;
    std::vector<Longitude<Degree>> lngs_;
    std::vector<bool> is_water_;

    size_t nCols(size_t rowIdx);
};
