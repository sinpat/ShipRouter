#pragma once

#include <Constants.hpp>
#include <functional>

template<class T>
using Ref = std::reference_wrapper<T>;

template<class T>
using CRef = std::reference_wrapper<const T>;

using Tags = std::unordered_map<std::string, std::string>;


struct Lat
{
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

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};

struct Lng
{
    explicit Lng(double value)
        : value_(value) {}

    Lng() = default;
    Lng(Lng&&) = default;
    Lng(const Lng&) = default;
    auto operator=(Lng&&) -> Lng& = default;
    auto operator=(const Lng&) -> Lng& = default;

    auto toDegree() const noexcept
        -> Lng
    {
        return Lng{value_ * 180 / PI};
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
