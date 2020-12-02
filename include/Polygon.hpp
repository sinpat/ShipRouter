#pragma once

#include <OSMNode.hpp>
#include <vector>

class NodeLookup;
class CoastlineLookup;

class Polygon
{
public:
    Polygon(std::vector<OSMNode> nodes);

    auto getNodes() const
        -> const std::vector<OSMNode>&;

    auto getNodes()
        -> std::vector<OSMNode>&;

private:
    std::vector<OSMNode> nodes_;
};


auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>;
