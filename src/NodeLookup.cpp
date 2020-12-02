#include <NodeLookup.hpp>


auto NodeLookup::addNode(std::uint64_t osmid,
                         double lon,
                         double lat,
                         Tags tags) noexcept
    -> void
{
    nodes_.try_emplace(osmid, osmid, lon, lat, std::move(tags));
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


auto NodeLookup::idsToNodes(const std::vector<std::uint64_t>& ids) const noexcept
    -> std::vector<OSMNode>
{

    std::vector<OSMNode> nodes;
    std::transform(std::begin(ids),
                   std::end(ids),
                   std::back_inserter(nodes),
                   [&](auto id) {
                       return std::move(nodes_.find(id)->second);
                   });

    return nodes;
}
