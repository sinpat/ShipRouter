#include <PBFExtractor.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <execution>
#include <fmt/core.h>

auto main() -> int
{
    auto [nodes, coastlines] = parsePBFFile("../data/antarctica-latest.osm.pbf");
    fmt::print("starting calculation of polygons...\n");
    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));
    // fmt::print("{}\n", polygons.size());
    fmt::print("should be water: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat<DegreeTag>{-60},
                                                            Lng<DegreeTag>{-80}); // is in land
                             }));
    fmt::print("should be water: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat<DegreeTag>{-70.15096965227654},
                                                            Lng<DegreeTag>{-1.8873336911201477}); // is in water
                             }));

    fmt::print("should be land: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat<DegreeTag>{-70.58638474216802},
                                                            Lng<DegreeTag>{-9.03076171875}); // is in water
                             }));

    fmt::print("should be land: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Lat<DegreeTag>{-64.19442343702701},
                                                            Lng<DegreeTag>{-57.81005859375}); // is in water
                             }));

    SphericalGrid grid{500};
    grid.filter(polygons);

}
