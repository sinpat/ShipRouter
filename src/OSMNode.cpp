#include <OSMNode.hpp>
#include <unordered_map>

OSMNode::OSMNode(double lon,
                 double lat,
                 Tags&& tags)
    : lon_(lon),
      lat_(lat),
      tags_(std::move(tags)) {}

auto OSMNode::getLon() const noexcept
    -> double
{
    return lon_;
}

auto OSMNode::getLat() const noexcept
    -> double
{
    return lat_;
}

auto OSMNode::getTags() const noexcept
    -> const Tags&
{
    return tags_;
}

auto OSMNode::getTags() noexcept
    -> Tags&
{
    return tags_;
}
