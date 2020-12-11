#pragma once

#include <CoastlineLookup.hpp>
#include <LatLng.hpp>
#include <NodeLookup.hpp>
#include <string_view>


auto parsePBFFile(std::string_view path) noexcept
    -> std::pair<NodeLookup, CoastlineLookup>;
