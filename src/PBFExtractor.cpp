#include <PBFExtractor.hpp>
#include <fmt/core.h>
#include <osmpbfreader.h>
#include <string_view>
#include <fmt/ranges.h>



namespace {

struct Visitor
{
    void node_callback(uint64_t osmid,
                       double lon,
                       double lat,
                       const CanalTP::Tags& tags)
    {
    }

    void way_callback(uint64_t osmid,
                      const CanalTP::Tags& tags,
                      const std::vector<uint64_t>& refs)
    {
	  if(auto iter = tags.find("natural");
		 iter != std::end(tags)
		 and iter->second == "coastline") {
            fmt::print("{}: {}\n",
					   osmid,
					   *iter);
        }
    }

    void relation_callback(uint64_t osmid,
                           const CanalTP::Tags& tags,
                           const CanalTP::References& refs) {}
};
} // namespace



auto parsePBFFile(std::string_view path)
    -> void
{
    Visitor v;
    CanalTP::read_osm_pbf(path.data(), v);
}

