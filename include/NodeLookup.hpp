#pragma once

#include <OSMNode.hpp>
#include <Utils.hpp>
#include <string_view>
#include <unordered_map>
#include <Polygon.hpp>

class CoastlineLookup;

class NodeLookup
{
public:
    auto addNode(std::uint64_t osmid,
                 double lon,
                 double lat,
                 Tags tags) noexcept
        -> void;

private:
    friend auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                                  NodeLookup&& node_lookup) noexcept
        -> std::vector<Polygon>;

  
    std::unordered_map<std::uint64_t, OSMNode> nodes_;
};
