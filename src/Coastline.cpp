#include <Coastline.hpp>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

Coastline::Coastline(Tags&& tags,
                     std::vector<std::uint64_t>&& refs)
    : tags_(std::move(tags)),
      refs_(std::move(refs)) {}

auto Coastline::getTags() const noexcept
    -> const Tags&
{
    return tags_;
}

auto Coastline::getTags() noexcept
    -> Tags&
{
    return tags_;
}

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
