#pragma once

#include <string_view>
#include <CoastlineLookup.hpp>
#include <NodeLookup.hpp>


auto parsePBFFile(std::string_view path) noexcept
    -> std::pair<NodeLookup, CoastlineLookup>;
