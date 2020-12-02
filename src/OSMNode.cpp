#include <OSMNode.hpp>
#include <unordered_map>

OSMNode::OSMNode(std::uint64_t id,
                 double lon,
                 double lat)
    : id_(id),
      lon_(lon),
      lat_(lat) {}

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

auto OSMNode::getId() const noexcept
    -> std::uint64_t
{
    return id_;
}

auto OSMNode::operator==(const OSMNode& other) const noexcept
    -> bool
{
    return lon_ == other.lon_ and lat_ == other.lat_;
}

auto OSMNode::operator!=(const OSMNode& other) const noexcept
    -> bool
{
    return !(*this == other);
}
