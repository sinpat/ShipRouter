#include <Dijkstra.hpp>
#include <Environment.hpp>
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

static auto handleUserInterrupt(int signal) noexcept
    -> void
{
    if(signal == SIGINT) {
        condition.notify_one();
    }
}

static auto waitForUserInterrupt() noexcept
    -> void
{
    std::unique_lock lock{mutex};
    condition.wait(lock);
    fmt::print("user has signaled to interrupt the program...");
    lock.unlock();
}


auto main() -> int
{

    auto environment = [] {
        auto environment_opt = loadEnv();
        if(!environment_opt) {
            fmt::print("the environment variables could not be understood\n");
            fmt::print("using default values\n");

            return Environment{9090,
                               "../data/antarctica-latest.osm.pbf",
                               10};
        }

        return environment_opt.value();
    }();


    auto [nodes, coastlines] = parsePBFFile(environment.getDataFile());
    fmt::print("calculating polygons...\n");

    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));

    fmt::print("building the grid...\n");
    SphericalGrid grid{environment.getNumberOfSphereNodes()};

    fmt::print("filtering land nodes...\n");
    grid.filter(polygons);

    Graph graph{std::move(grid)};

	
	//handle sigint such that the user can stop the server
    std::signal(SIGINT, handleUserInterrupt);
    std::signal(SIGPIPE, [](int /**/) {});

    ServiceManager manager{Pistache::Address{Pistache::IP::any(),
                                             environment.getPort()},
                           graph};
    try {
        fmt::print("started server, listening at: {}\n",
                   environment.getPort());
        std::cout << std::flush;

        manager.serveThreaded();

        waitForUserInterrupt();

        manager.shutdown();

        fmt::print("shutting down server");

    } catch(const std::exception& e) {
        fmt::print("catched exception: {}", e.what());
    }
}
