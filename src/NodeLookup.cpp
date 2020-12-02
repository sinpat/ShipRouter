#include <NodeLookup.hpp>


auto NodeLookup::addNode(std::uint64_t osmid,
                         double lon,
                         double lat,
                         Tags tags) noexcept
    -> void
{
    nodes_.try_emplace(osmid, osmid, lon, lat, std::move(tags));
}
