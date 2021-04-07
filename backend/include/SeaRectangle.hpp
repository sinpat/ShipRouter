#include <LatLng.hpp>
#include <array>

class SeaRectangle
{
    constexpr SeaRectangle(Latitude<Degree> top_left_lat,
                           Longitude<Degree> top_left_lng,
                           Latitude<Degree> bottom_right_lat,
                           Longitude<Degree> bottom_right_lng) noexcept
        : top_left_lat_(top_left_lat),
          top_left_lng_(top_left_lng),
          bottom_right_lat_(bottom_right_lat),
          bottom_right_lng_(bottom_right_lng) {}

    constexpr auto isIn(Latitude<Degree> lat,
                        Longitude<Degree> lng) noexcept
    {
        return top_left_lat_ < lat
            and top_left_lng_ < lng
            and bottom_right_lat_ > lat
            and bottom_right_lng_ > lng;
    }

private:
    Latitude<Degree> top_left_lat_;
    Longitude<Degree> top_left_lng_;
    Latitude<Degree> bottom_right_lat_;
    Longitude<Degree> bottom_right_lng_;
};

constexpr auto SEA_RECTANGLES = std::array<SeaRectangle, 0>{};
