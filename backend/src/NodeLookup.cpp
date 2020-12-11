#include <NodeLookup.hpp>


auto NodeLookup::addNode(std::uint64_t osmid,
                         double lon,
                         double lat) noexcept
    -> void
{
    nodes_.try_emplace(osmid, osmid, lon, lat);
}
