#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>
#include <PBFExtractor.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <osmpbfreader.h>
#include <string_view>



namespace {

struct Visitor
{
    void node_callback(uint64_t osmid,
                       double lon,
                       double lat,
                       const CanalTP::Tags& tags)
    {
        node_lookup_.addNode(osmid, lon, lat, tags);
    }

    void way_callback(uint64_t osmid,
                      const CanalTP::Tags& tags,
                      const std::vector<uint64_t>& refs)
    {
        if(auto iter = tags.find("natural");
           iter != std::end(tags)
           and iter->second == "coastline") {
            coastline_lookup_.addCoastline(refs);
        }
    }

    void relation_callback(uint64_t osmid,
                           const CanalTP::Tags& tags,
                           const CanalTP::References& refs) {}

    NodeLookup node_lookup_;
    CoastlineLookup coastline_lookup_;
};
} // namespace



auto parsePBFFile(std::string_view path) noexcept
    -> std::pair<NodeLookup, CoastlineLookup>
{
    Visitor v;
    CanalTP::read_osm_pbf(path.data(), v);

    return std::pair{std::move(v.node_lookup_),
                     std::move(v.coastline_lookup_)};
}
