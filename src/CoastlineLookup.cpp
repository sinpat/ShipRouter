#include <Coastline.hpp>
#include <CoastlineLookup.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

auto CoastlineLookup::addCoastline(std::uint64_t osmid,
                                   Tags tags,
                                   std::vector<std::uint64_t> refs) noexcept
    -> void
{
    coastlines_.try_emplace(osmid, std::move(tags), std::move(refs));
}

auto CoastlineLookup::getCoastline(std::uint64_t osmid) const noexcept
    -> std::optional<CRef<Coastline>>
{
    auto iter = coastlines_.find(osmid);
    if(iter == std::end(coastlines_)) {
        return std::nullopt;
    }

    return std::cref(iter->second);
}

auto CoastlineLookup::deleteCoastline(std::uint64_t osmid) noexcept
    -> void
{
    coastlines_.erase(osmid);
}


auto CoastlineLookup::calculatePolygons() const noexcept
    -> std::vector<Polygon>
{
    std::unordered_map<std::uint64_t, Coastline> coastlines;

    for(const auto& [id, coastline] : coastlines_) {
        if(coastline.getRefs().empty()) {
            continue;
        }

        auto last = coastline.getRefs().back();
        auto first = coastline.getRefs().front();

        auto iter = coastlines.find(first);

        if(iter == std::end(coastlines)) {
            coastlines.emplace(last, coastline);
            continue;
        }

        auto old_way = std::move(iter->second.getRefs());
        const auto& new_refs = coastline.getRefs();

        std::copy(std::begin(new_refs) + 1,
                  std::end(new_refs),
                  std::back_inserter(old_way));

        coastlines.erase(first);
        coastlines.try_emplace(last, std::move(old_way));
    }

    std::vector<Polygon> polygons;
    std::transform(std::make_move_iterator(std::begin(coastlines)),
                   std::make_move_iterator(std::end(coastlines)),
                   std::back_inserter(polygons),
                   [](auto elem) {
                       return elem.second;
                   });

    return polygons;
}
