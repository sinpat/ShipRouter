#include <PBFExtractor.hpp>
#include <fmt/core.h>

auto main() -> int
{
    auto [nodes, coastlines] = parsePBFFile("../data/antarctica-latest.osm.pbf");
    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));

    for(auto poly : polygons) {
        if(poly.getNodes().empty()) {
            fmt::print("reeeeeeeeeeeeeeee\n");
        }
        if(poly.getNodes().front().getId() != poly.getNodes().back().getId()) {
            fmt::print("reeeeeeeeeeeeeeee\n");
        }
    }
}
