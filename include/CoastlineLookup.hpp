#pragma once

#include <Coastline.hpp>
#include <NodeLookup.hpp>
#include <Polygon.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class CoastlineLookup
{
public:
    auto addCoastline(std::uint64_t osmid,
                      std::vector<std::uint64_t> refs) noexcept
        -> void;

    auto getCoastline(std::uint64_t osmid) const noexcept
        -> std::optional<CRef<Coastline>>;

    auto deleteCoastline(std::uint64_t osmid) noexcept
        -> void;

    auto calculatePolygons(const NodeLookup& node_lookup) const noexcept
        -> std::vector<Polygon>;

private:
    std::unordered_map<std::uint64_t, Coastline> coastlines_;
};
