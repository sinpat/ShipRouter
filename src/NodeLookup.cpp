#include <NodeLookup.hpp>


auto NodeLookup::addNode(std::uint64_t osmid,
                         double lon,
                         double lat,
                         std::unordered_map<std::string, std::string>&& tags) noexcept
    -> void
{
    nodes_.try_emplace(osmid, lon, lat, std::move(tags));
}

auto NodeLookup::getNode(std::uint64_t osmid) const noexcept
    -> std::optional<CRef<OSMNode>>
{
    auto iter = nodes_.find(osmid);

    if(iter == std::end(nodes_)) {
        return std::nullopt;
    }

    return std::cref(iter->second);
}

auto NodeLookup::deleteNode(std::uint64_t osmid) noexcept
    -> void
{
    nodes_.erase(osmid);
}
