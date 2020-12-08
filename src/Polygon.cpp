#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>
#include <OSMNode.hpp>
#include <Polygon.hpp>
#include <SphericalPoint.hpp>

Polygon::Polygon(std::vector<OSMNode> nodes)
    : nodes_(std::move(nodes)) {}

auto Polygon::getNodes() const
    -> const std::vector<OSMNode>&
{
    return nodes_;
}

auto Polygon::getNodes()
    -> std::vector<OSMNode>&
{
    return nodes_;
}

auto Polygon::pointInPolygon(double lng, double lat) const
    -> bool
{
    // create ray with given point and random other point
    const auto ray = SphericalLine(
        SphericalPoint(lng, lat),
        SphericalPoint(0, 0));
    // iterate over all edges of polygon and count intersections with ray
    const auto n_nodes = nodes_.size();
    auto hits = 0;
    for(size_t i = 0; i < n_nodes; i++) {
        const auto p1 = &nodes_[i];
        const auto p2 = &nodes_[i + 1 % n_nodes];
        const auto poly_edge = SphericalLine(
            SphericalPoint(p1->getLon(), p1->getLat()),
            SphericalPoint(p2->getLon(), p2->getLat()));
        if(ray.crosses(poly_edge)) {
            hits++;
        }
    }
    // point lies in polygon, if the edges are hit an even number of times, but not never
    return hits != 0 && hits % 2 == 0;
}

auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>
{
    auto coastlines = std::move(coastline_lookup.coastlines_);

    std::vector<Polygon> polygons;
    while(!coastlines.empty()) {
        auto [first, current_line] = std::move(*coastlines.begin());

        while(current_line.getRefs().front() != current_line.getRefs().back()) {
            auto current_last = current_line.getRefs().back();

            auto iter = coastlines.find(current_last);
            if(iter == std::end(coastlines)) {
                current_line.getRefs().emplace_back(current_line.getRefs().front());
                continue;
            }

            auto [inner_first, append_line] = std::move(*iter);
            coastlines.erase(current_last);

            auto new_nodes = std::move(current_line.getRefs());
            std::copy(std::begin(append_line.getRefs()) + 1,
                      std::end(append_line.getRefs()),
                      std::back_inserter(new_nodes));

            current_line = std::move(new_nodes);
        }

        coastlines.erase(first);

        std::vector<OSMNode> polygon_nodes;
        std::transform(std::begin(current_line.getRefs()),
                       std::end(current_line.getRefs()),
                       std::back_inserter(polygon_nodes),
                       [&](auto id) {
                           return std::move(node_lookup.nodes_.find(id)->second);
                       });

        polygons.emplace_back(std::move(polygon_nodes));
    }

    return polygons;
}
