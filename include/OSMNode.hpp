
#pragma once

#include <Utils.hpp>
#include <unordered_map>

class OSMNode
{
public:
    OSMNode(double lon,
            double lat,
            Tags&& tags);

    auto getLon() const noexcept
        -> double;

    auto getLat() const noexcept
        -> double;

    auto getTags() const noexcept
        -> const Tags&;

    auto getTags() noexcept
        -> Tags&;

private:
    double lon_;
    double lat_;
    Tags tags_;
};
