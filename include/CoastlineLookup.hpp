#pragma once

#include <Coastline.hpp>
#include <NodeLookup.hpp>
#include <Polygon.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

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

auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>;
