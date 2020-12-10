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

    auto sphericalToGrid(Latitude<Radian> theta, Longitude<Radian> phi) const noexcept
        -> std::pair<std::size_t, std::size_t>;

    auto getRowNeighbours(size_t m, size_t n) const noexcept
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto getUpperNeighbours(size_t m, size_t n) const noexcept
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto getLowerNeighbours(size_t m, size_t n) const noexcept
        -> std::vector<std::pair<std::size_t, std::size_t>>;
    auto getNeighbours(size_t m, size_t n) const noexcept
        -> std::vector<size_t>;

    auto getNeighbours(std::size_t id) const noexcept
        -> std::vector<size_t>;

    auto gridToID(size_t m, size_t n) const noexcept
        -> size_t;

    //only valid if from and to are neigbours
    auto distanceBetween(NodeId from, NodeId to) const noexcept
        -> Distance;

    auto idToGrid(size_t id) const noexcept
	  -> std::pair<size_t, size_t>;

    auto indexIsWater(std::size_t idx) const noexcept
        -> bool;

    auto indexIsLand(std::size_t idx) const noexcept
        -> bool;

    auto filter(const std::vector<Polygon>& polygons) noexcept
        -> void;

    auto size() const noexcept
        -> std::size_t;

private:
    double a_;
    size_t n_rows_;
    double d_phi_;
    std::vector<size_t> first_index_of_;
    std::vector<Latitude<Degree>> lats_;
    std::vector<Longitude<Degree>> lngs_;
    std::vector<bool> is_water_;

    auto nCols(size_t row_idx) const
        -> std::size_t;
};
