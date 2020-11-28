#include <OSMNode.hpp>
#include <unordered_map>

OSMNode::OSMNode(double lon,
                 double lat,
                 std::unordered_map<std::string, std::string>&& tags)
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
    -> const std::unordered_map<std::string, std::string>&
{
    return tags_;
}

auto OSMNode::getTags() noexcept
    -> std::unordered_map<std::string, std::string>&
{
    return tags_;
}
