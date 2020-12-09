#pragma once

#include <Constants.hpp>
#include <cmath>

// clang-format off
struct RadianTag{};
struct DegreeTag{};
// clang-format on

template<class T>
constexpr static inline auto is_tag = std::is_same_v<T, RadianTag> or std::is_same_v<T, DegreeTag>;


template<class Tag>
class Lat
{
    static_assert(is_tag<Tag>, "lat template needs to be a tag");

public:
    explicit Lat(double value) noexcept
        : value_(value) {}

    Lat() = default;
    Lat(Lat&&) noexcept = default;
    Lat(const Lat&) = default;
    auto operator=(Lat&&) noexcept -> Lat& = default;
    auto operator=(const Lat&) -> Lat& = default;

    template<class Q = Tag>
    auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, RadianTag>, Lat<DegreeTag>>
    {
        return Lat<DegreeTag>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, DegreeTag>, Lat<RadianTag>>
    {
        return Lat<RadianTag>{value_ * PI / 180};
    }

    template<class Q = Tag>
    auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, DegreeTag>,
                            Lat<DegreeTag>>
    {
        return Lat<DegreeTag>{std::fmod(value_, 90) - 90};
    }

    auto operator-(double other) const noexcept
        -> Lat<Tag>
    {
        return Lat<Tag>{value_ - other};
    }

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};

template<class Tag>
class Lng
{
    static_assert(is_tag<Tag>, "lng template needs to be a tag");

public:
    explicit Lng(double value)
        : value_(value) {}

    Lng() = default;
    Lng(Lng&&) noexcept = default;
    Lng(const Lng&) = default;
    auto operator=(Lng&&) noexcept -> Lng& = default;
    auto operator=(const Lng&) -> Lng& = default;

    auto operator-(double other) const noexcept
        -> Lng<Tag>
    {
        return Lng<Tag>{value_ - other};
    }

    template<class Q = Tag>
    auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, RadianTag>,
                            Lng<DegreeTag>>
    {
        return Lng<DegreeTag>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, DegreeTag>,
                            Lng<DegreeTag>>
    {
        return Lng<DegreeTag>{std::fmod(value_, 180) - 180};
    }

    template<class Q = Tag>
    auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, DegreeTag>,
                            Lng<RadianTag>>
    {
        return Lng<RadianTag>{value_ * PI / 180};
    }

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};
