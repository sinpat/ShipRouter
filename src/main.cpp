#include <Dijkstra.hpp>
#include <PBFExtractor.hpp>
#include <ServiceManager.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <csignal>
#include <cstdint>
#include <execution>
#include <fmt/core.h>
#include <fmt/ranges.h>

static std::condition_variable condition;
static std::mutex mutex;

constexpr static inline auto PORT = 9999;

static auto handleUserInterrupt(int signal)
    -> void
{
    if(signal == SIGINT) {
        condition.notify_one();
    }
}

static auto waitForUserInterrupt()
    -> void
{
    std::unique_lock lock{mutex};
    condition.wait(lock);
    fmt::print("user has signaled to interrupt the program...");
    lock.unlock();
}

auto main() -> int
{
    std::signal(SIGINT, handleUserInterrupt);
    std::signal(SIGPIPE, [](int /**/) {});

    auto [nodes, coastlines] = parsePBFFile("../data/antarctica-latest.osm.pbf");
    fmt::print("starting calculation of polygons...\n");
    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));
    SphericalGrid grid{1000};
    grid.filter(polygons);

    ServiceManager manager{Pistache::Address{Pistache::IP::any(),
                                             PORT},
                           grid};
    try {
        fmt::print("started server, listening at: {}",
                   PORT);

        manager.serveThreaded();

        waitForUserInterrupt();

        manager.shutdown();

        fmt::print("shutting down server");

    } catch(const std::exception& e) {
        fmt::print("catched exception: {}", e.what());
    }
}
