#pragma once

#include <OSMNode.hpp>
#include <vector>

class NodeLookup;
class CoastlineLookup;

class Polygon
{
public:
    Polygon(const std::vector<OSMNode>& nodes);

    auto pointInPolygon(Lat<DegreeTag> lat, Lng<DegreeTag> lng) const
        -> bool;

    auto numberOfPoints() const
        -> std::size_t;

    auto getLatAndLng() const
        -> std::vector<std::pair<double, double>>;

private:
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> z_;
};


auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>;
