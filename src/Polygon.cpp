#include <OSMNode.hpp>
#include <Polygon.hpp>

Polygon::Polygon(std::vector<OSMNode> nodes)
    : nodes_(std::move(nodes)) {}

auto Polygon::getNodes() const
    -> const std::vector<OSMNode>&
{
    return nodes_;
}

auto Polygon::getNodes()
    -> std::vector<OSMNode>&
{
    return nodes_;
}
