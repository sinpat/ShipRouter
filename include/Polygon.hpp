#pragma once

#include <OSMNode.hpp>
#include <vector>

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
