#include <Dijkstra.hpp>
#include <Graph.hpp>
#include <Range.hpp>
#include <SphericalGrid.hpp>
#include <Vector3D.hpp>
#include <fmt/ranges.h>
#include <iostream>
#include <nonstd/span.hpp>
#include <numeric>
#include <queue>
#include <unordered_set>


Graph::Graph(SphericalGrid&& g)
    : offset_(g.size() + 1, 0),
      snap_settled_(g.size(), false),
      levels(g.size(), 0),
      grid_(std::move(g))
{
    for(auto id : utils::range(grid_.size())) {
        if(!grid_.indexIsLand(id)) {
            auto neigs = grid_.getNeighbours(id);

            std::sort(std::begin(neigs),
                      std::end(neigs));
            auto remove_iter =
                std::unique(std::begin(neigs),
                            std::end(neigs));

            neigs.erase(remove_iter, std::end(neigs));

            std::vector<Edge> neig_dist;
            std::transform(std::begin(neigs),
                           std::end(neigs),
                           std::back_inserter(neig_dist),
                           [&](auto neig) {
                               auto [start_lat, start_lng] = grid_.idToLatLng(id);
                               auto [dest_lat, dest_lng] = grid_.idToLatLng(neig);
                               auto distance = ::distanceBetween(start_lat, start_lng, dest_lat, dest_lng);

                               return Edge{id, neig, static_cast<Distance>(distance), std::nullopt};
                           });

            auto numEdgesOld = edges_.size();
            for(auto i = 0; i < neig_dist.size(); ++i) {
                auto edge_id = numEdgesOld + i;
                sorted_edge_ids_.emplace_back(edge_id);
                sorted_edge_ids_with_source_.emplace_back(edge_id, id);
            }
            edges_ = concat(std::move(edges_),
                            std::move(neig_dist));
        }

        offset_[id + 1] = edges_.size();

        auto [m, n] = grid_.idToGrid(id);
        ns_.emplace_back(n);
        ms_.emplace_back(m);
    }
    fmt::print("Checking if we have inverse edges for every edge...\n");
    for(auto i = 0; i < edges_.size(); ++i) {
        auto inv_edge = inverseEdge(i);
    }

    //insert dummy at the end
    // edges_.emplace_back(std::numeric_limits<NodeId>::max(), UNREACHABLE, std::nullopt);
}

auto Graph::idToLat(NodeId id) const noexcept
    -> Latitude<Degree>
{
    return grid_.lats_[id];
}

auto Graph::idToLng(NodeId id) const noexcept
    -> Longitude<Degree>
{
    return grid_.lngs_[id];
}

auto Graph::idToM(NodeId id) const noexcept
    -> std::size_t
{
    return ms_[id];
}

auto Graph::idToN(NodeId id) const noexcept
    -> std::size_t
{
    return ns_[id];
}

auto Graph::isValidId(NodeId id) const noexcept
    -> bool
{
    return id < size();
}

auto Graph::size() const noexcept
    -> std::size_t
{
    return grid_.lats_.size();
}

auto Graph::relaxEdgeIds(NodeId node) const noexcept
    -> nonstd::span<const EdgeId>
{
    const auto start_offset = offset_[node];
    const auto end_offset = offset_[node + 1];
    const auto start = &sorted_edge_ids_[start_offset];
    const auto end = &sorted_edge_ids_[end_offset];
    return nonstd::span{start, end};
}

auto Graph::gridToId(std::size_t m, std::size_t n) const noexcept
    -> NodeId
{
    return grid_.gridToID(m, n);
}

auto Graph::isLandNode(NodeId node) const noexcept
    -> bool
{
    return !grid_.is_water_[node];
}

const Edge& Graph::getEdge(EdgeId edge_id) const noexcept
{
    return edges_[edge_id];
}

Level Graph::getLevel(NodeId node) const noexcept
{
    return levels[node];
}


auto Graph::getRowGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    auto on_grid = grid_.getRowNeighbours(m, n);
    std::vector<NodeId> ids;
    std::transform(std::begin(on_grid),
                   std::end(on_grid),
                   std::back_inserter(ids),
                   [&](auto pair) {
                       auto [m, n] = pair;
                       return gridToId(m, n);
                   });

    return ids;
}

auto Graph::getLowerGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    auto on_grid = grid_.getLowerNeighbours(m, n);
    std::vector<NodeId> ids;
    std::transform(std::begin(on_grid),
                   std::end(on_grid),
                   std::back_inserter(ids),
                   [&](auto pair) {
                       auto [m, n] = pair;
                       return gridToId(m, n);
                   });

    return ids;
}

auto Graph::getUpperGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    auto on_grid = grid_.getUpperNeighbours(m, n);
    std::vector<NodeId> ids;
    std::transform(std::begin(on_grid),
                   std::end(on_grid),
                   std::back_inserter(ids),
                   [&](auto pair) {
                       auto [m, n] = pair;
                       return gridToId(m, n);
                   });

    return ids;
}

auto Graph::getGridNeigboursOf(std::size_t m, std::size_t n) const noexcept
    -> std::vector<NodeId>
{
    return concat(getUpperGridNeigboursOf(m, n),
                  getLowerGridNeigboursOf(m, n),
                  getRowGridNeigboursOf(m, n));
}

auto Graph::getSnapNodeCandidate(Latitude<Degree> lat,
                                 Longitude<Degree> lng) const noexcept
    -> NodeId
{
    const auto [m, n] = grid_.sphericalToGrid(lat.toRadian(), lng.toRadian());

    const auto id = gridToId(m, n);

    fmt::print("index: {}\n", id);

    std::vector<NodeId> candidates;
    std::vector<NodeId> touched_nodes;

    if(!isLandNode(id)) {
        candidates.emplace_back(id);
    }

    auto workstack = getGridNeigboursOf(m, n);

    while(!workstack.empty()) {
        const auto candidate = workstack.back();
        workstack.pop_back();
        touched_nodes.emplace_back(candidate);

        if(!isLandNode(candidate)) {
            candidates.emplace_back(candidate);
            continue;
        }

        if(snap_settled_[candidate]) {
            continue;
        }

        workstack = concat(std::move(workstack),
                           getGridNeigboursOf(ms_[candidate],
                                              ns_[candidate]));
        snap_settled_[candidate] = true;
    }

    for(auto touched : touched_nodes) {
        snap_settled_[touched] = false;
    }

    return *std::min_element(std::cbegin(candidates),
                             std::cend(candidates),
                             [&](auto lhs, auto rhs) {
                                 auto lhs_lat = grid_.lats_[lhs];
                                 auto lhs_lng = grid_.lngs_[lhs];

                                 auto rhs_lat = grid_.lats_[rhs];
                                 auto rhs_lng = grid_.lngs_[rhs];

                                 return ::distanceBetween(lat, lng, lhs_lat, lhs_lng)
                                     < ::distanceBetween(lat, lng, rhs_lat, rhs_lng);
                             });
}

auto Graph::snapToGridNode(Latitude<Degree> lat,
                           Longitude<Degree> lng) const noexcept
    -> NodeId
{
    auto candidate = getSnapNodeCandidate(lat, lng);

    std::priority_queue candidates(
        [&](auto id1, auto id2) {
            return ::distanceBetween(lat, lng,
                                     grid_.lats_[id1],
                                     grid_.lngs_[id1])
                > ::distanceBetween(lat, lng,
                                    grid_.lats_[id2],
                                    grid_.lngs_[id2]);
        },
        std::vector{candidate});

    while(true) {
        const auto best_before_insert = candidates.top();
        for(auto edge_id : relaxEdgeIds(best_before_insert)) {
            candidates.emplace(edges_[edge_id].target);
        }
        const auto best_after_insert = candidates.top();

        if(best_before_insert == best_after_insert) {
            break;
        }
    }

    return candidates.top();
}

std::vector<std::pair<NodeId, NodeId>> Graph::randomSTPairs(uint amount) const noexcept
{
    std::vector<std::pair<NodeId, NodeId>> st_pairs;
    for(auto i = 0; i < amount - 1; ++i) {
        auto source = rand() % size();
        auto target = rand() % size();
        st_pairs.emplace_back(source, target);
    }
    return st_pairs;
}

Path Graph::unwrapEdge(EdgeId edge_id, NodeId target) const noexcept
{
    const Edge& edge = edges_[edge_id];
    if(!edge.wrapped_edges) {
        if(edge.source == target) {
            return std::vector<NodeId>{edge.target};
        } else if(edge.target == target) {
            return std::vector<NodeId>{edge.source};
        }
    }
    auto [edge1, edge2] = edge.wrapped_edges.value();
    auto path_edge2 = unwrapEdge(edge2, target);
    auto path = unwrapEdge(edge1, path_edge2[0]);
    path.insert(path.end(), path_edge2.begin(), path_edge2.end());
    return path;
}

// === stuff for ch and contraction === //

void Graph::contract() noexcept
{
    fmt::print("Starting graph contraction...\n");
    Dijkstra dijkstra{*this};
    while(!fully_contracted) {
        // fmt::print("Graph contains {} nodes\n", size());
        // fmt::print("Edges:\n");
        // for(auto i = 0; i < size(); ++i) {
        //     fmt::print("{} is water: {}\n", i, grid_.is_water_[i]);
        //     for(auto edge_id : relaxEdgeIds(i)) {
        //         auto edge = edges_[edge_id];
        //         fmt::print("{} -> {}: {} {}\n", edge.source, edge.target, edge.dist, edge.wrapped_edges.has_value());
        //     }
        //     fmt::print("\n");
        // }
        contractionStep(dijkstra);
    }
    fmt::print("Done contracting with {} levels\n", current_level);
}

void Graph::contractionStep(Dijkstra& dijkstra) noexcept
{
    fmt::print("Graph has {} edges\n", edges_.size());
    /*
    * 1. create independent set of nodes
    * 2. for each node: 
    *      calculate distances from and to all neighbors and node edge diff
    * 3. sort by edge diff ascending
    * 4. Create shortcuts for the first n nodes (with lowest edge diff)
    * 5. Insert new edges into existing graph
      */

    // 1.
    auto indep_nodes = independentSet();
    // fmt::print("Independent set contains {} nodes: {}\n", indep_nodes.size(), indep_nodes);
    if(indep_nodes.empty()) {
        fully_contracted = true;
        return;
    }

    // 2.
    // holds the edge_diff and new_edges for every node in the independent set
    std::vector<std::tuple<NodeId, int32_t, std::vector<Edge>>> newEdgeCandidates;
    for(auto node : indep_nodes) {
        // fmt::print("Contracting node {}\n", node);
        std::vector<Edge> new_edges;
        auto edge_ids = relaxEdgeIds(node);

        for(auto i = 0; i < edge_ids.size(); ++i) {
            auto edge_id1 = edge_ids[i];
            auto source = edges_[edge_id1].target;
            if(nodeContracted(source)) {
                continue;
            }
            // shortest path from neigh to all other neighbors
            for(auto j = i + 1; j < edge_ids.size(); ++j) {
                auto edge_id2 = edge_ids[j];
                auto target = edges_[edge_id2].target;
                if(nodeContracted(target)) {
                    continue;
                }
                auto cost = edges_[edge_id1].dist + edges_[edge_id2].dist;
                if(dijkstra.shortestPathContainsU(source, target, node, cost)) {
                    // fmt::print("found shortcut with cost {}\n", cost);
                    new_edges.emplace_back(
                        Edge{source,
                             target,
                             cost,
                             std::pair{inverseEdge(edge_id1), edge_id2}});
                    // insert inverse shortcut
                    new_edges.emplace_back(
                        Edge{target,
                             source,
                             cost,
                             std::pair{inverseEdge(edge_id2), edge_id1}});
                }
            }
        }
        auto edge_diff = new_edges.size() - edge_ids.size();
        newEdgeCandidates.emplace_back(node, edge_diff, new_edges);
    }
    // fmt::print("Done checking for possible shortcuts\n");

    // 3.
    std::sort(newEdgeCandidates.begin(), newEdgeCandidates.end(), [](const auto& lhs, const auto& rhs) {
        return std::get<1>(lhs) < std::get<1>(rhs);
    });

    // 4.
    current_level++;
    const auto median = std::get<1>(newEdgeCandidates[newEdgeCandidates.size() / 2]);
    std::vector<Edge> toInsert;
    for(auto [node, edge_diff, new_edges] : newEdgeCandidates) {
        if(edge_diff <= median) {
            levels[node] = current_level;
            toInsert = concat(std::move(toInsert), std::move(new_edges));
        }
    }
    // fmt::print("levels {}\n", levels);

    // 5.
    fmt::print("Contraction step {}: adding {} new shortcuts\n", current_level, toInsert.size());
    insertEdges(toInsert);
}

std::vector<NodeId> Graph::independentSet() const noexcept
{
    std::vector<bool> visited(size(), false);
    std::vector<NodeId> indepNodes;

    for(auto i = 0; i < size(); i++) {
        if(levels[i] == 0 && !visited[i]) {
            auto edge_ids = relaxEdgeIds(i);
            for(auto edge_id : edge_ids) {
                const Edge& e = edges_[edge_id];
                visited[e.target] = true;
            }
            indepNodes.emplace_back(i);
        }
    }
    return indepNodes;
}

void Graph::insertEdges(std::vector<Edge> toInsert)
{
    // fmt::print("Updating graph with new edges...\n");
    for(auto new_edge : toInsert) {
        auto source = new_edge.source;
        // fmt::print("adding shortcut from {} to {}\n", source, new_edge.target);
        // 1. insert new edges
        // edges_.insert(edges_.end(),
        //               new_edges.begin(),
        //               new_edges.end());

        // 2. clear sortedEdgeIds

        // 3. consider new edges in offset array
        for(auto i = source + 1; i < offset_.size(); i++) {
            offset_[i] += 1;
        }
        // 3 (alt). build new offset array
        /*
        for(auto edge : edges_) {
            offset_[edge.source]++;
        }
        for(auto i = 1; i < offset_.size(); i++) {
            offset_[i] += offset_[i - 1];
        }
        offset_.emplace_back(edges_.size());
        */

        // insert new edges
        auto numEdgesOld = edges_.size();
        // for(auto i = 0; i < new_edges.size(); ++i) {
        auto edge_id = numEdgesOld;
        edges_.emplace_back(new_edge);
        sorted_edge_ids_.emplace_back(edge_id); // only added to extend size of vector
        sorted_edge_ids_with_source_.emplace_back(edge_id, source);
        // }
    }
    // sort edges
    std::sort(
        sorted_edge_ids_with_source_.begin(),
        sorted_edge_ids_with_source_.end(),
        [](auto pair1, auto pair2) {
            return pair1.second < pair2.second;
        });
    for(auto i = 0; i < sorted_edge_ids_with_source_.size(); i++) {
        sorted_edge_ids_[i] = sorted_edge_ids_with_source_[i].first;
    }
}

bool Graph::nodeContracted(NodeId id) const noexcept
{
    return levels[id] > 0;
}

EdgeId Graph::inverseEdge(EdgeId edge_id) const noexcept
{
    const Edge& edge = edges_[edge_id];
    const auto candidates = relaxEdgeIds(edge.target);
    for(auto cand_id : candidates) {
        if(edges_[cand_id].target == edge.source) {
            return cand_id;
        }
    }
    fmt::print("Did not find inverse edge!! Something is wrong with the graph.\n");
}
