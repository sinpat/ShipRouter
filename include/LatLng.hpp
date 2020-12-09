#pragma once

#include <Constants.hpp>
#include <cmath>

class Lat
{
public:
    explicit Lat(double value)
        : value_(value) {}

    Lat() = default;
    Lat(Lat&&) = default;
    Lat(const Lat&) = default;
    auto operator=(Lat&&) -> Lat& = default;
    auto operator=(const Lat&) -> Lat& = default;

    auto toDegree() const noexcept
        -> Lat
    {
        return Lat{value_ * 180 / PI};
    }

    auto toRadian() const noexcept
        -> Lat
    {
        return Lat{value_ * PI / 180};
    }

    auto normalizeDegree() const noexcept
        -> Lat
    {
        return Lat{std::fmod(value_, 90) - 90};
    }

    auto operator-(double other) const noexcept
        -> Lat
    {
        return Lat{value_ - other};
    }

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};

class Lng
{
public:
    explicit Lng(double value)
        : value_(value) {}

    Lng() = default;
    Lng(Lng&&) = default;
    Lng(const Lng&) = default;
    auto operator=(Lng&&) -> Lng& = default;
    auto operator=(const Lng&) -> Lng& = default;

    auto operator-(double other) const noexcept
        -> Lng
    {
        return Lng{value_ - other};
    }

    auto toDegree() const noexcept
        -> Lng
    {
        return Lng{value_ * 180 / PI};
    }

    auto normalizeDegree() const noexcept
        -> Lng
    {
        return Lng{std::fmod(value_, 180) - 180};
    }

    auto toRadian() const noexcept
        -> Lng
    {
        return Lng{value_ * PI / 180};
    }

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};
