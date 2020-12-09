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
    constexpr explicit Latitude(double value) noexcept
        : value_(value) {}

    constexpr Latitude() = default;
    constexpr Latitude(Latitude&&) noexcept = default;
    constexpr Latitude(const Latitude&) = default;
    constexpr auto operator=(Latitude&&) noexcept -> Latitude& = default;
    constexpr auto operator=(const Latitude&) -> Latitude& = default;

    operator double() const
    {
        return value_;
    }

    template<class Q = Tag>
    constexpr auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Radian>, Latitude<Degree>>
    {
        return Latitude<Degree>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    constexpr auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>, Latitude<Radian>>
    {
        return Latitude<Radian>{value_ * PI / 180};
    }

    template<class Q = Tag>
    constexpr auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Latitude<Degree>>
    {
        return Latitude<Degree>{std::fmod(value_, 90) - 90};
    }


    constexpr auto getValue() const
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
    constexpr explicit Longitude(double value)
        : value_(value) {}

    constexpr Longitude() = default;
    constexpr Longitude(Longitude&&) noexcept = default;
    constexpr Longitude(const Longitude&) = default;
    constexpr auto operator=(Longitude&&) noexcept -> Longitude& = default;
    constexpr auto operator=(const Longitude&) -> Longitude& = default;

    constexpr operator double() const
    {
        return value_;
    }


    template<class Q = Tag>
    constexpr auto toDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Radian>,
                            Longitude<Degree>>
    {
        return Longitude<Degree>{value_ * 180 / PI};
    }

    template<class Q = Tag>
    constexpr auto normalizeDegree() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Longitude<Degree>>
    {
        return Longitude<Degree>{std::fmod(value_, 180) - 180};
    }

    template<class Q = Tag>
    constexpr auto toRadian() const noexcept
        -> std::enable_if_t<std::is_same_v<Q, Degree>,
                            Longitude<Radian>>
    {
        return Longitude<Radian>{value_ * PI / 180};
    }

    constexpr auto getValue() const
        -> double
    {
        return value_;
    }

private:
    double value_;
};


template<class Tag>
auto distanceBetween(Latitude<Tag> lat_start,
                     Longitude<Tag> lng_start,
                     Latitude<Tag> lat_dest,
                     Longitude<Tag> lng_dest) noexcept
    -> std::enable_if_t<is_tag<Tag>, double>
{
    auto [phi_1, phi_2, delta_phi, delta_lambda] = [&]() constexpr
    {
        if constexpr(std::is_same_v<Tag, Degree>) {

            const auto phi_1 = lat_start.toRadian();
            const auto phi_2 = lat_dest.toRadian();

            const auto delta_phi = (lat_dest.getValue() - lat_start.getValue()) * (PI / 180.0);
            const auto delta_lambda = (lng_dest.getValue() - lng_start.getValue()) * (PI / 180.0);


            return std::tuple{phi_1, phi_2, delta_phi, delta_lambda};

        } else {
            const auto phi_1 = lat_start;
            const auto phi_2 = lat_dest;

            const auto delta_phi = (lat_dest.getValue() - lat_start.getValue());
            const auto delta_lambda = (lng_dest.getValue() - lng_start.getValue());

            return std::tuple{phi_1, phi_2, delta_phi, delta_lambda};
        }
    }
    ();

    const auto a = std::sin(delta_phi / 2) * std::sin(delta_phi)
        + std::cos(phi_1) * std::cos(phi_2)
            * std::sin(delta_lambda / 2) * std::sin(delta_lambda / 2);

    const auto c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return EARTH_RADIUS_IN_METERS * c;
}
