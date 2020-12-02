#pragma once

#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class Coastline
{
public:
    Coastline(std::vector<std::uint64_t> refs);

    auto getRefs() const noexcept
        -> const std::vector<std::uint64_t>&;
    auto getRefs() noexcept
        -> std::vector<std::uint64_t>&;

private:
    std::vector<std::uint64_t> refs_;
};
