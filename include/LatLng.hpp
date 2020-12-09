#pragma once

#include <Constants.hpp>
#include <cmath>

// clang-format off
struct Radian{};
struct Degree{};
// clang-format on

template<class T>
constexpr static inline auto is_tag = std::is_same_v<T, Radian> or std::is_same_v<T, Degree>;


template<class Tag>
class Latitude
{
    static_assert(is_tag<Tag>, "lat template needs to be a tag");

public:
    explicit Latitude(double value) noexcept
        : value_(value) {}

    Latitude() = default;
    Latitude(Latitude&&) noexcept = default;
    Latitude(const Latitude&) = default;
    auto operator=(Latitude&&) noexcept -> Latitude& = default;
    auto operator=(const Latitude&) -> Latitude& = default;

    operator double() const
    {
        return value_;
    }

    template<class Q = Tag>
    auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Radian>, Latitude<Degree>>
    {
        return Latitude<Degree>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>, Latitude<Radian>>
    {
        return Latitude<Radian>{value_ * PI / 180};
    }

    template<class Q = Tag>
    auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Latitude<Degree>>
    {
        return Latitude<Degree>{std::fmod(value_, 90) - 90};
    }

    auto operator-(double other) const noexcept
        -> Latitude<Tag>
    {
        return Latitude<Tag>{value_ - other};
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
class Longitude
{
    static_assert(is_tag<Tag>, "lng template needs to be a tag");

public:
    explicit Longitude(double value)
        : value_(value) {}

    Longitude() = default;
    Longitude(Longitude&&) noexcept = default;
    Longitude(const Longitude&) = default;
    auto operator=(Longitude&&) noexcept -> Longitude& = default;
    auto operator=(const Longitude&) -> Longitude& = default;

    operator double() const
    {
        return value_;
    }

    auto operator-(double other) const noexcept
        -> Longitude<Tag>
    {
        return Longitude<Tag>{value_ - other};
    }

    template<class Q = Tag>
    auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Radian>,
                            Longitude<Degree>>
    {
        return Longitude<Degree>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Longitude<Degree>>
    {
        return Longitude<Degree>{std::fmod(value_, 180) - 180};
    }

    template<class Q = Tag>
    auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Longitude<Radian>>
    {
        return Longitude<Radian>{value_ * PI / 180};
    }

    auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};
