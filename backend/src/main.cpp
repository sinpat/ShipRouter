#include <CHDijkstra.hpp>
#include <Dijkstra.hpp>
#include <Environment.hpp>
#include <PBFExtractor.hpp>
#include <ServiceManager.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <execution>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fstream>
#include <iostream>

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
    std::cout << "user has signaled to interrupt the program..." << std::endl;
    lock.unlock();
}

void benchmark(
    std::string file_name,
    std::vector<std::pair<NodeId, NodeId>> st_pairs,
    std::function<DijkstraPath(NodeId, NodeId)> fn)
{
    fmt::print("Starting benchmark for {}\n", file_name);
    std::string log = "source,target,query_time,q_pops,distance,path\n";
    for(auto [s, t] : st_pairs) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        DijkstraPath p = fn(s, t);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto time_diff_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        log += fmt::format("{},{},{},", s, t, time_diff_ms);
        if(p) {
            auto [path, dist, pops] = p.value();
            log += fmt::format("{},{},", pops, dist);
            for(auto i = 0; i < path.size(); i++) {
                log += fmt::format("{}", path[i]);
                if(i != path.size() - 1) {
                    log += "->";
                }
            }
            log += "\n";
        } else {
            log += ",,\n";
        }
    }

    std::ofstream myfile;
    myfile.open(fmt::format("../results/{}.csv", file_name));
    myfile << log;
    myfile.close();
}


auto main() -> int
{
    auto environment = [] {
        auto environment_opt = loadEnv();
        if(!environment_opt) {
            std::cout << "the environment variables could not be understood" << std::endl;
            std::cout << "using default values" << std::endl;

            return Environment{9090,
                               "../data/antarctica-latest.osm.pbf",
                               //    "../data/planet-coastlines.pbf",
                               100};
        }

        return environment_opt.value();
    }();


    std::cout << "Parsing pbf file..." << std::endl;
    auto [nodes, coastlines] = parsePBFFile(environment.getDataFile());
    std::cout << "calculating polygons..." << std::endl;

    auto polygons = calculatePolygons(std::move(coastlines),
                                      std::move(nodes));

    std::cout << "building the grid..." << std::endl;
    SphericalGrid grid{environment.getNumberOfSphereNodes()};

    std::cout << "filtering land nodes ... " << std::endl;
    std::chrono::steady_clock::time_point begin_filter = std::chrono::steady_clock::now();
    grid.filter(polygons);
    std::chrono::steady_clock::time_point end_filter = std::chrono::steady_clock::now();
    std::cout << "Filtering took " << std::chrono::duration_cast<std::chrono::seconds>(end_filter - begin_filter).count() << "[s]" << std::endl;

    Graph graph{std::move(grid)};

    // get n random source-target tuples
    std::vector<std::pair<NodeId, NodeId>> st_pairs = graph.randomSTPairs(100);
    // run normal dijkstra on these tuples and save to file
    Dijkstra dijkstra{graph};
    benchmark("normal", st_pairs, [&](NodeId s, NodeId t) {
        return dijkstra.findRoute(s, t);
    });
    std::chrono::steady_clock::time_point begin_contract = std::chrono::steady_clock::now();
    graph.contract(); // contract graph
    std::chrono::steady_clock::time_point end_contract = std::chrono::steady_clock::now();
    std::cout << "Contracting took " << std::chrono::duration_cast<std::chrono::seconds>(end_contract - begin_contract).count() << "[s]" << std::endl;
    // run ch-dijkstra on same tuples and save to different file
    CHDijkstra ch_dijkstra{graph};
    benchmark("ch", st_pairs, [&](NodeId s, NodeId t) {
        return ch_dijkstra.findRoute(s, t);
    });


    //handle sigint such that the user can stop the server
    std::signal(SIGINT, handleUserInterrupt);
    std::signal(SIGPIPE, [](int /**/) {});

    ServiceManager manager{Pistache::Address{Pistache::IP::any(),
                                             environment.getPort()},
                           graph};
    try {
        fmt::print("started server, listening at: {}",
                   environment.getPort());
        std::cout << std::endl;

        manager.serveThreaded();

        waitForUserInterrupt();

        manager.shutdown();

        std::cout << "shutting down server" << std::endl;

    } catch(const std::exception& e) {
        fmt::print("caught exception: {}\n", e.what());
    }
}
