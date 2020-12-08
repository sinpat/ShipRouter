#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>
#include <OSMNode.hpp>
#include <Polygon.hpp>
#include <Range.hpp>
#include <SphericalPoint.hpp>
#include <Vector3D.hpp>
#include <numeric>

Polygon::Polygon(const std::vector<OSMNode>& nodes)
{
    for(const auto& n : nodes) {
        auto lat = n.getLat();
        auto lng = n.getLon();
        auto [x, y, z] = latLngTo3D(lat, lng);
        x_.emplace_back(x);
        y_.emplace_back(y);
        z_.emplace_back(z);
    }
}

auto Polygon::pointInPolygon(double lat, double lng) const
    -> bool
{
    const auto n_vertices = numberOfPoints() - 1;
    auto range = utils::range(n_vertices);
    Vector3D p{lat, lng};
    // get vectors from p to each vertex
    std::vector<Vector3D> vec_to_vertex;
    std::transform(std::begin(range),
                   std::end(range),
                   std::back_inserter(vec_to_vertex),
                   [&](auto idx) {
                       return p - Vector3D{x_[idx], y_[idx], z_[idx]};
                   });

    vec_to_vertex.emplace_back(vec_to_vertex[0]);


    auto sum = std::accumulate(std::begin(range),
                               std::end(range),
                               0.0,
                               [&](auto current, auto idx) {
                                   const auto& first = vec_to_vertex[idx];
                                   const auto& second = vec_to_vertex[idx + 1];
                                   return current + first.angleBetween(second, p);
                               });

    return std::abs(sum) > M_PI;
}

auto Polygon::getLatAndLng() const
    -> std::vector<std::pair<double, double>>
{
    std::vector<std::pair<double, double>> ret_vec;
    auto range = utils::range(numberOfPoints());

    std::transform(std::begin(range),
                   std::end(range),
                   std::back_inserter(ret_vec),
                   [&](auto idx) {
                       return vec3DtoLatLong(x_[idx], y_[idx], z_[idx]);
                   });

    return ret_vec;
}

auto Polygon::numberOfPoints() const
    -> std::size_t
{
    return x_.size();
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
