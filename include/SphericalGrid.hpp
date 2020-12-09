#pragma once
#include <cmath>
#include <vector>

class SphericalGrid
{
public:
    SphericalGrid();
    auto get_nodes() const -> std::vector<std::pair<double, double>>;

private:
    std::vector<std::pair<double, double>> nodes_;
};