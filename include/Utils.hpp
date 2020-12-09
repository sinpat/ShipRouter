#pragma once

#include <functional>

template<class T>
using Ref = std::reference_wrapper<T>;

template<class T>
using CRef = std::reference_wrapper<const T>;

using Tags = std::unordered_map<std::string, std::string>;


struct Lat
{
    explicit Lat(double value)
        : value(value) {}

    Lat() = default;
    Lat(Lat&&) = default;
    Lat(const Lat&) = default;
    auto operator=(Lat&&) -> Lat& = default;
    auto operator=(const Lat&) -> Lat& = default;

    double value;
};

struct Lng
{
    explicit Lng(double value)
        : value(value) {}

    Lng() = default;
    Lng(Lng&&) = default;
    Lng(const Lng&) = default;
    auto operator=(Lng&&) -> Lng& = default;
    auto operator=(const Lng&) -> Lng& = default;

    double value;
};
