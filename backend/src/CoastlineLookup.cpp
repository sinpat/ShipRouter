#include <Coastline.hpp>
#include <CoastlineLookup.hpp>
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
