#include <Dijkstra.hpp>
#include <PBFExtractor.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <ServiceManager.hpp>
#include <execution>
#include <fmt/core.h>
#include <fmt/ranges.h>

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
                                 return poly.pointInPolygon(Latitude<Degree>{-60},
                                                            Longitude<Degree>{-80}); // is in land
                             }));
    fmt::print("should be water: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Latitude<Degree>{-70.15096965227654},
                                                            Longitude<Degree>{-1.8873336911201477}); // is in water
                             }));

    fmt::print("should be land: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Latitude<Degree>{-70.58638474216802},
                                                            Longitude<Degree>{-9.03076171875}); // is in water
                             }));

    fmt::print("should be land: {}\n",
               std::count_if(std::execution::par,
                             std::begin(polygons),
                             std::end(polygons),
                             [](const auto& poly) {
                                 return poly.pointInPolygon(Latitude<Degree>{-64.19442343702701},
                                                            Longitude<Degree>{-57.81005859375}); // is in water
                             }));

    SphericalGrid grid{10000};
    grid.filter(polygons);

    const auto& lats = grid.getLats();
    const auto& lngs = grid.getLngs();

    Dijkstra dijk{grid};

    auto [path, distance] = dijk.findRoute(1000, 1500).value();

    fmt::print("Route: {}\n", fmt::join(path, ","));
    for(auto id : path) {
        fmt::print("[{},{}],\n", lngs[id].getValue(), lats[id].getValue());
    }
}
