#pragma once

#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class Coastline
{
public:
    Coastline(Tags&& tags,
              std::vector<std::uint64_t>&& refs);

    auto getTags() const noexcept
        -> const Tags&;
    auto getTags() noexcept
        -> Tags&;

    auto getRefs() const noexcept
        -> const std::vector<std::uint64_t>&;
    auto getRefs() noexcept
        -> std::vector<std::uint64_t>&;

private:
    Tags tags_;
    std::vector<std::uint64_t> refs_;
};
