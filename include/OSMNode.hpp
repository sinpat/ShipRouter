
#pragma once

#include <Utils.hpp>
#include <unordered_map>

class OSMNode
{
public:
    OSMNode(std::uint64_t id,
            double lon,
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

    auto getId() const noexcept
        -> std::uint64_t;

private:
    std::uint64_t id_;
    double lon_;
    double lat_;
    Tags tags_;
};
