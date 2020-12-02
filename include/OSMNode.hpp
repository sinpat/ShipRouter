
#pragma once

#include <Utils.hpp>
#include <unordered_map>

class OSMNode
{
public:
    OSMNode(std::uint64_t id,
            double lon,
            double lat);

    auto getLon() const noexcept
        -> double;

    auto getLat() const noexcept
        -> double;

    auto getId() const noexcept
        -> std::uint64_t;

    auto operator==(const OSMNode& other) const noexcept
        -> bool;

    auto operator!=(const OSMNode& other) const noexcept
        -> bool;

private:
    std::uint64_t id_;
    double lon_;
    double lat_;
};

namespace std {

template<>
struct hash<OSMNode>
{
    auto operator()(const OSMNode& node) const noexcept
        -> std::size_t
    {
        auto h1 = std::hash<double>()(node.getLat());
        auto h2 = std::hash<double>()(node.getLon());
        return (h1 ^ (h2 << 1));
    }
};

} // namespace std
