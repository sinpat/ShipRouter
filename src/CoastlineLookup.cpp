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
