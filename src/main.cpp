#include <PBFExtractor.hpp>
#include <fmt/core.h>

auto main() -> int
{
    [[maybe_unused]] auto [nodes, coastlines] =
        parsePBFFile("../data/antarctica-latest.osm.pbf");
}
