#include <string_view>
#include <PBFExtractor.hpp>
#include <osmpbfreader.h>
#include <fmt/core.h>



namespace {

struct Visitor
{
    void node_callback(uint64_t osmid, double lon, double lat, const CanalTP::Tags& tags)
    {
        fmt::print("{}\n", osmid);
    }

    void way_callback(uint64_t osmid, const CanalTP::Tags& tags, const std::vector<uint64_t>& refs) {}

    void relation_callback(uint64_t osmid, const CanalTP::Tags& tags, const CanalTP::References& refs) {}
};
} // namespace



auto parsePBFFile(std::string_view path)
    -> void
{
    Visitor v;
	CanalTP::read_osm_pbf(path.data(), v);
}




