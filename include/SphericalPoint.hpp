#pragma once
#include <cmath>

constexpr static inline auto EARTH_RADIUS_IN_METERS = 6'371'000.0;
constexpr static inline auto PI = 3.14159265358979323846;

class SphericalPoint
{
public:
    constexpr SphericalPoint(double lng, double lat) noexcept
        : lng_(lng), lat_(lat) {}

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
    friend class SphericalLine;
    double lng_;
    double lat_;
};

class SphericalLine
{
public:
    constexpr SphericalLine(SphericalPoint start,
                            SphericalPoint end) noexcept
        : start_(start),
          end_(end) {}

    auto crosses(const SphericalLine& other) const noexcept
        -> bool;

    auto bearing() const noexcept
        -> double
    {
        const auto phi_1 = start_.lat_;
        const auto phi_2 = end_.lat_;
        const auto lambda_1 = start_.lng_;
        const auto lambda_2 = end_.lng_;

        const auto y = std::sin(lambda_2 - lambda_1) * std::cos(phi_2);
        const auto x = std::cos(phi_1) * std::sin(phi_2)
            - std::sin(phi_1) * std::cos(phi_2) * std::cos(lambda_2 - lambda_1);
        const auto O = std::atan2(y, x);

        return std::fmod((O * 180 / PI + 360), 360);
    }



private:
    SphericalPoint start_;
    SphericalPoint end_;
};
