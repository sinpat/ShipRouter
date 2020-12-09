#include <SphericalGrid.hpp>

SphericalGrid::SphericalGrid()
{
    std::vector<std::pair<double, double>> nodes;
    auto a = 4 * M_PI / 5000;
    auto d = sqrt(a);
    auto M_theta = round(M_PI / d);
    auto d_theta = M_PI / M_theta;
    auto d_phi = a / d_theta;
    for(size_t m = 0; m < M_theta; m++) {
        auto theta = M_PI * (m + 0.5) / M_theta;
        auto M_phi = round(2 * M_PI * sin(theta) / d_phi);
        for(size_t n = 0; n < M_phi; n++) {
            auto phi = 2 * M_PI * n / M_phi;
            nodes.emplace_back(theta, phi);
        }
    }
    nodes_ = nodes;
}

auto SphericalGrid::get_nodes() const -> std::vector<std::pair<double, double>>
{
    return nodes_;
}