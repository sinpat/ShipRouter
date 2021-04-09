#include <Dijkstra.hpp>
#include <LatLng.hpp>
#include <ServiceManager.hpp>
#include <SphericalGrid.hpp>
#include <Utils.hpp>
#include <nlohmann/json.hpp>
#include <pistache/endpoint.h>
#include <pistache/mime.h>
#include <pistache/router.h>

using Pistache::Http::ResponseWriter;


ServiceManager::ServiceManager(const Pistache::Address& address,
                               const Graph& grid)
    : Pistache::Http::Endpoint(address),
      grid_(grid),
      dijkstra_(grid_)
{
    auto opts = Pistache::Http::Endpoint::options()
                    .flags(Pistache::Tcp::Options::ReuseAddr)
                    .threads(8);
    init(opts);

    setUpGETRoutes();
    setHandler(router_.handler());
}


auto ServiceManager::snapNode(Latitude<Degree> lat, Longitude<Degree> lng) const
    -> nlohmann::json
{
    auto snapped = grid_.snapToGridNode(lat, lng);
    auto new_lat = grid_.idToLat(snapped);
    auto new_lng = grid_.idToLng(snapped);

    nlohmann::json result;

    result["id"] = snapped;
    result["lat"] = new_lat.getValue();
    result["lng"] = new_lng.getValue();

    return result;
}

auto ServiceManager::getRoute(NodeId source, NodeId target)
    -> std::optional<nlohmann::json>
{
    if(!grid_.isValidId(source) or !grid_.isValidId(target)) {
        return std::nullopt;
    }

    std::unique_lock lock{dijkstra_mtx_};
    auto routing_result = dijkstra_.findRoute(source, target);
    lock.unlock();

    nlohmann::json result;
    if(!routing_result) {
        result["lats"] = nlohmann::json::array();
        result["lngs"] = nlohmann::json::array();
        result["distance"] = UNREACHABLE;

        return result;
    }

    auto [path, distance, _] = std::move(routing_result.value());

    std::vector<double> lats;
    std::vector<double> lngs;
    for(auto i : path) {
        auto lat = grid_.idToLat(i);
        auto lng = grid_.idToLng(i);
        lats.emplace_back(lat);
        lngs.emplace_back(lng);
    }

    result["lats"] = std::move(lats);
    result["lngs"] = std::move(lngs);
    result["distance"] = distance;

    return result;
}

auto ServiceManager::setUpGETRoutes()
    -> void
{
    using Pistache::Rest::Request;
    using Pistache::Http::ResponseWriter;
    using Pistache::Rest::Routes::Get;
    using namespace Pistache;
    Get(router_, "/snap/",
        [=](const Request& request, ResponseWriter response) {
            response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
            const auto& query = request.query();
            if(!query.has("lat") or !query.has("lng")) {

                response.send(Http::Code::Bad_Request);
                return Rest::Route::Result::Failure;
            }

            const auto lat_str = request.query().get("lat").get();
            const auto lng_str = request.query().get("lng").get();

            try {
                const auto lat = Latitude<Degree>(std::stod(lat_str));
                const auto lng = Longitude<Degree>(std::stod(lng_str));
                const auto snapped = snapNode(lat, lng);

                response.send(Http::Code::Ok, snapped.dump());

                return Rest::Route::Result::Ok;
            } catch(...) {

                response.send(Http::Code::Bad_Request);
                return Rest::Route::Result::Failure;
            }
        });

    Get(router_, "/route/",
        [=](const Request& request, ResponseWriter response) {
            response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
            const auto& query = request.query();
            if(!query.has("source") or !query.has("target")) {
                response.send(Http::Code::Bad_Request);
                return Rest::Route::Result::Failure;
            }

            const auto source_str = request.query().get("source").get();
            const auto target_str = request.query().get("target").get();

            try {
                const auto source = std::stoul(source_str);
                const auto target = std::stoul(target_str);
                const auto route_opt = getRoute(source, target);

                if(!route_opt) {
                    return Rest::Route::Result::Failure;
                }

                const auto& route = route_opt.value();

                response.send(Http::Code::Ok, route.dump());

                return Rest::Route::Result::Ok;
            } catch(...) {
                response.send(Http::Code::Bad_Request);
                return Rest::Route::Result::Failure;
            }
        });
}
