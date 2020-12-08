#pragma once

#include <cmath>
#include <tuple>

constexpr static inline auto EARTH_RADIUS_IN_METERS = 6'371'000.0;
constexpr static inline auto PI = 3.14159265358979323846;

using Vector3D = std::tuple<double, double, double>;

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

    auto toVector() const noexcept
        -> Vector3D
    {
        return Vector3D{std::cos(lat_) * std::cos(lng_),
                        std::cos(lat_) * std::sin(lng_),
                        std::sin(lat_)};
    }


private:
    friend class SphericalSegment;
    double lng_;
    double lat_;
};


auto dotProduct(const Vector3D& first, const Vector3D& second) noexcept
    -> double
{
    auto [x1, y1, z1] = first;
    auto [x2, y2, z2] = second;

    return x1 * x2 + y1 * y2 + z1 * z2;
}

auto crossProduct(const Vector3D& first, const Vector3D& second) noexcept
    -> Vector3D
{
    auto [x1, y1, z1] = first;
    auto [x2, y2, z2] = second;

    return Vector3D{y1 * z2 - z1 * y2,
                    z1 * x2 - x1 * z2,
                    x1 * y2 - y2 * x2};
}

auto length(const Vector3D& first) noexcept
    -> double
{
    auto [x, y, z] = first;

    return std::sqrt(x * x + y * y + z * z);
}

auto angleBetween(const Vector3D& first,
                  const Vector3D& second,
                  const Vector3D& plain_normal) noexcept
    -> double
{
    const auto sign = dotProduct(crossProduct(first, second),
                                 plain_normal)
            >= 0
        ? 1.0
        : -1.0;

    const auto sin_theta = length(crossProduct(first, second)) * sign;

    const auto cos_theta = dotProduct(first, second);

    return std::atan2(sin_theta, cos_theta);
}
