#pragma once

#include <OSMNode.hpp>
#include <vector>

class NodeLookup;
class CoastlineLookup;

class Polygon
{
public:
    Polygon(const std::vector<OSMNode>& nodes);

    auto pointInPolygon(double lat, double lng) const
        -> bool;

    auto numberOfPoints() const
        -> std::size_t;

private:
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> z_;
};


auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>;