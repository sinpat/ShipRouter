
#pragma once

#include <unordered_map>

class OSMNode
{
public:
    OSMNode(double lon,
            double lat,
            std::unordered_map<std::string, std::string>&& tags);

    auto getLon() const noexcept
        -> double;

    auto getLat() const noexcept
        -> double;

    auto getTags() const noexcept
        -> const std::unordered_map<std::string, std::string>&;

    auto getTags() noexcept
        -> std::unordered_map<std::string, std::string>&;

private:
    double lon_;
    double lat_;
    std::unordered_map<std::string, std::string> tags_;
};
