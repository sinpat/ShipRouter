#include <Coastline.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

Coastline::Coastline(std::vector<std::uint64_t> refs)
    : refs_(std::move(refs)) {}

auto Coastline::getRefs() const noexcept
    -> const std::vector<std::uint64_t>&
{
    return refs_;
}

auto Coastline::getRefs() noexcept
    -> std::vector<std::uint64_t>&
{
    return refs_;
}
