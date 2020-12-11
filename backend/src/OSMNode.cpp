#include <LatLng.hpp>
#include <OSMNode.hpp>
#include <unordered_map>

OSMNode::OSMNode(std::uint64_t id,
                 double lon,
                 double lat)
    : id_(id),
      lon_(lon),
      lat_(lat) {}

auto OSMNode::getLon() const noexcept
    -> Longitude<Degree>
{
    return lon_;
}

auto OSMNode::getLat() const noexcept
    -> Latitude<Degree>
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
    return id_ == other.id_;
}

auto OSMNode::operator!=(const OSMNode& other) const noexcept
    -> bool
{
    return !(*this == other);
}
