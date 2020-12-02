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

    auto getNode(std::uint64_t osmid) const noexcept
        -> std::optional<CRef<OSMNode>>;

    auto deleteNode(std::uint64_t osmid) noexcept
        -> void;

    auto idsToNodes(const std::vector<std::uint64_t>& ids) const noexcept
        -> std::vector<OSMNode>;

private:
    friend auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                                  NodeLookup&& node_lookup) noexcept
        -> std::vector<Polygon>;

  
    std::unordered_map<std::uint64_t, OSMNode> nodes_;
};
