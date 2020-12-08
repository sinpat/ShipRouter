
#pragma once

#include <cmath>
#include <tuple>

constexpr static inline auto EARTH_RADIUS_IN_METERS = 6'371'000.0;
constexpr static inline auto PI = 3.14159265358979323846;

class Vector3D
{
public:
    Vector3D(double lat, double lng)
        : x_(std::cos(lat) * std::cos(lng)),
          y_(std::cos(lat) * std::sin(lng)),
          z_(std::sin(lat)) {}

    Vector3D() = default;
    Vector3D(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    auto operator=(const Vector3D&) -> Vector3D& = default;
    auto operator=(Vector3D&&) -> Vector3D& = default;

    constexpr Vector3D(double x, double y, double z)
        : x_(x), y_(y), z_(z) {}

    constexpr auto dotProduct(const Vector3D& other) const noexcept
        -> double
    {
        return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
    }

    constexpr auto crossProduct(const Vector3D& second) const noexcept
        -> Vector3D
    {
        auto [x1, y1, z1] = *this;
        auto [x2, y2, z2] = second;

        return Vector3D{y1 * z2 - z1 * y2,
                        z1 * x2 - x1 * z2,
                        x1 * y2 - y2 * x2};
    }

    auto length() const noexcept
        -> double
    {
        return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    constexpr auto operator-(const Vector3D& other) const noexcept
        -> Vector3D
    {
        return Vector3D{
            x_ - other.x_,
            y_ - other.y_,
            z_ - other.z_};
    }

    auto angleBetween(const Vector3D& other,
                      const Vector3D& plain_normal) const noexcept
        -> double
    {
        const auto sign = crossProduct(other).dotProduct(plain_normal) >= 0 ? 1.0 : -1.0;
        const auto sin_theta = crossProduct(other).length() * sign;
        const auto cos_theta = dotProduct(other);

        return std::atan2(sin_theta, cos_theta);
    }

private:
    double x_;
    double y_;
    double z_;
};
