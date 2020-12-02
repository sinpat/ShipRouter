#pragma once

#include <Coastline.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <Polygon.hpp>

class NodeLookup;

class CoastlineLookup
{
public:
    auto addCoastline(std::vector<std::uint64_t> refs) noexcept
        -> void;

private:
    friend auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                                  NodeLookup&& node_lookup) noexcept
        -> std::vector<Polygon>;

    std::unordered_map<std::uint64_t, Coastline> coastlines_;
};
