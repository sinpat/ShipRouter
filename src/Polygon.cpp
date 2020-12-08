#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>
#include <OSMNode.hpp>
#include <Polygon.hpp>
#include <SphericalPoint.hpp>
#include <Vector3D.hpp>

Polygon::Polygon(const std::vector<OSMNode>& nodes)
{
    for(const auto& n : nodes) {
        lats_.emplace_back(n.getLat());
        lngs_.emplace_back(n.getLon());
    }
}

auto Polygon::pointInPolygon(double lat, double lng) const
    -> bool
{
    const auto n_vertices = numberOfPoints() - 1;
    const auto p = Vector3D(lat, lng);
    // get vectors from p to each vertex
    std::vector<Vector3D> vec_to_vertex(n_vertices);
    for(size_t v = 0; v < n_vertices; v++) {
        vec_to_vertex[v] = p - Vector3D{lats_[v], lngs_[v]};
    }
    vec_to_vertex.emplace_back(vec_to_vertex[0]);

    // sum angles
    double sum = 0;
    for(size_t v = 0; v < n_vertices; v++) {
        sum += vec_to_vertex[v].angleBetween(vec_to_vertex[v + 1], p);
    }
    return std::abs(sum) > M_PI;
}

auto Polygon::numberOfPoints() const
    -> std::size_t
{
    return lats_.size();
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
