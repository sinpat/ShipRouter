
#pragma once

#include <Constants.hpp>
#include <LatLng.hpp>
#include <Utils.hpp>
#include <cmath>
#include <tuple>

class Vector3D
{
public:
    Vector3D(Lat lat, Lng lng)
        : x_(std::cos(lat.getValue()) * std::cos(lng.getValue())),
          y_(std::cos(lat.getValue()) * std::sin(lng.getValue())),
          z_(std::sin(lat.getValue())) {}

    Vector3D() = default;
    Vector3D(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    auto operator=(const Vector3D&) -> Vector3D& = default;
    auto operator=(Vector3D&&) -> Vector3D& = default;

    constexpr Vector3D(double x, double y, double z)
        : x_(x), y_(y), z_(z) {}

    auto normalize() const noexcept
        -> Vector3D
    {
        const auto l = length();
        if(l == 0 or l == 1) {
            return *this;
        }

        const auto x = x_ / l;
        const auto y = y_ / l;
        const auto z = z_ / l;

        return Vector3D{x, y, z};
    }

    constexpr auto dotProduct(const Vector3D& other) const noexcept
        -> double
    {
        return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
    }

    constexpr auto crossProduct(const Vector3D& other) const noexcept
        -> Vector3D
    {
        return Vector3D{y_ * other.z_ - z_ * other.y_,
                        z_ * other.x_ - x_ * other.z_,
                        x_ * other.y_ - other.y_ * other.x_};
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
