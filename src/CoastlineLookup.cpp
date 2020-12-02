#include <Coastline.hpp>
#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>
#include <Utils.hpp>
#include <fmt/core.h>
#include <string>
#include <unordered_map>
#include <vector>

auto CoastlineLookup::addCoastline(std::vector<std::uint64_t> refs) noexcept
    -> void
{
    coastlines_.try_emplace(refs.front(), std::move(refs));
}

auto calculatePolygons(CoastlineLookup&& coastline_lookup,
                       NodeLookup&& node_lookup) noexcept
    -> std::vector<Polygon>
{
    std::unordered_map<std::uint64_t, Coastline> coastlines;
    for(auto [_, coastline] : std::move(coastline_lookup.coastlines_)) {
        auto first = coastline.getRefs().front();
        coastlines.emplace(first, std::move(coastline));
    }

    std::vector<Polygon> polygons;
    while(!coastlines.empty()) {
        auto [first, current_line] = std::move(*coastlines.begin());

        while(current_line.getRefs().front() != current_line.getRefs().back()) {
            auto current_last = current_line.getRefs().back();

            auto [inner_first, append_line] = std::move(*coastlines.find(current_last));
            coastlines.erase(current_last);

            auto new_nodes = std::move(current_line.getRefs());
            std::copy(std::begin(append_line.getRefs()) + 1,
                      std::end(append_line.getRefs()),
                      std::back_inserter(new_nodes));

            current_line = std::move(new_nodes);
        }

        coastlines.erase(first);

        auto polygon_nodes = node_lookup.idsToNodes(current_line.getRefs());
		polygons.emplace_back(std::move(polygon_nodes));
    }


    return polygons;
}
