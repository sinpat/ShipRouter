#pragma once

#include <cmath>
#include <tuple>
#include <Constants.hpp>


class SphericalPoint
{
public:
    constexpr SphericalPoint(double lng, double lat) noexcept
        : lng_(lng), lat_(lat) {}

    SphericalPoint(double x, double y, double z) noexcept
        : lng_(std::atan2(y, x)),
          lat_(std::atan2(z, std::sqrt(x * x + y * y))) {}

    auto distance(const SphericalPoint& other) const noexcept
        -> double
    {
        const auto phi_1 = lat_ * (PI / 180.0);
        const auto phi_2 = other.lat_ * (PI / 180.0);

        const auto delta_phi = (other.lat_ - lat_) * (PI / 180.0);
        const auto delta_lambda = (other.lng_ - lng_) * (PI / 180.0);

        const auto a = std::sin(delta_phi / 2) * std::sin(delta_phi / 2)
            + std::cos(phi_1) * std::cos(phi_2)
                * std::sin(delta_lambda / 2) * std::sin(delta_lambda / 2);

        const auto c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));


        return EARTH_RADIUS_IN_METERS * c;
    }

private:
    double lng_;
    double lat_;
};
