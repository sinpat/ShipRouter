#include <PBFExtractor.hpp>
#include <Vector3D.hpp>
#include <fmt/core.h>

auto main() -> int
{
    auto [nodes, coastlines] = parsePBFFile("../data/antarctica-latest.osm.pbf");
    fmt::print("starting calculation of polygons...\n");
    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));
    // fmt::print("{}\n", polygons.size());
    fmt::print("in land: {}\n",
               std::count_if(std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat{-76.10079606754577},
                                                            Lng{15.8203125}); // is in land
                             }));
    fmt::print("in water: {}\n",
               std::count_if(std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat{-77.50411917973987},
															Lng{-44.6484375}); // is in water
                             }));
}
