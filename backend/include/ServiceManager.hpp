#pragma once

#include <Dijkstra.hpp>
#include <SphericalGrid.hpp>
#include <nlohmann/json.hpp>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/net.h>
#include <pistache/router.h>
#include <string>
#include <vector>

class ServiceManager : public Pistache::Http::Endpoint
{
public:
    ServiceManager(const Pistache::Address& address,
                   const SphericalGrid& grid);

private:
    auto snapNode(Latitude<Degree> lat, Longitude<Degree> lng) const
        -> nlohmann::json;

    auto getRoute(NodeId source, NodeId target)
        -> std::optional<nlohmann::json>;

    auto setUpGETRoutes()
        -> void;

private:
    Pistache::Rest::Router router_;
    const SphericalGrid& grid_;

    std::mutex dijkstra_mtx_;
    Dijkstra dijkstra_;
};
