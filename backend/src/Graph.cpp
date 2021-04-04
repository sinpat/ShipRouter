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

                               return Edge{neig, static_cast<Distance>(distance), std::nullopt};
                           });

            auto numEdgesOld = edges_.size();
            for(auto i = 0; i < neig_dist.size(); ++i) {
                auto edge_id = numEdgesOld + i;
                sorted_edge_ids_.emplace_back(edge_id);
                sorted_edge_ids_with_source_.emplace_back(id, edge_id);
            }
            edges_ = concat(std::move(edges_),
                            std::move(neig_dist));
        }

        offset_[id + 1] = edges_.size();

        auto [m, n] = grid_.idToGrid(id);
        ns_.emplace_back(n);
        ms_.emplace_back(m);
    }

    //insert dummy at the end
    edges_.emplace_back(std::numeric_limits<NodeId>::max(), UNREACHABLE, std::nullopt);
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

auto Graph::relaxEdges(NodeId node) const noexcept
    -> nonstd::span<const Edge>
{
    const auto start_offset = offset_[node];
    const auto end_offset = offset_[node + 1];
    const auto* start = &edges_[start_offset];
    const auto* end = &edges_[end_offset];

    return nonstd::span{start, end};
}

auto Graph::relaxEdgesWithIds(NodeId node) const noexcept
    -> std::pair<nonstd::span<const Edge>, std::vector<EdgeId>>
{
    const auto start_offset = offset_[node];
    const auto end_offset = offset_[node + 1];
    const auto* start = &edges_[start_offset];
    const auto* end = &edges_[end_offset];

    std::vector<EdgeId> IDs(end_offset - start_offset);
    std::iota(IDs.begin(), IDs.end(), start_offset);

    return std::pair{
        nonstd::span{start, end},
        IDs};
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
        for(auto e : relaxEdges(best_before_insert)) {
            candidates.emplace(e.target);
        }
        const auto best_after_insert = candidates.top();

        if(best_before_insert == best_after_insert) {
            break;
        }
    }

    return candidates.top();
}

// === stuff for ch and contraction === //

void Graph::contract() noexcept
{
    fmt::print("Starting graph contraction...\n");
    while(!fully_contracted) {
        fmt::print("Graph contains {} nodes\n", size());
        fmt::print("Edges:\n");
        for(auto i = 0; i < size(); ++i) {
            fmt::print("{} is water: {}\n", i, grid_.is_water_[i]);
            for(auto edge : relaxEdges(i)) {
                fmt::print("{} -> {}: {} {}\n", i, edge.target, edge.dist, edge.wrapped_edges.has_value());
            }
            fmt::print("\n");
        }
        contractionStep();
    }
    fmt::print("Done contracting\n.");
}

void Graph::contractionStep() noexcept
{
    fmt::print("Starting contraction step {}\n", current_level);
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
    fmt::print("Independent set contains {} nodes: {}\n", indep_nodes.size(), indep_nodes);
    if(indep_nodes.empty()) {
        fully_contracted = true;
        return;
    }

    // 2.
    Dijkstra dijkstra{*this};
    std::vector<std::pair<int32_t, std::vector<std::pair<NodeId, Edge>>>> newEdgeCandidates;
    for(auto node : indep_nodes) {
        fmt::print("Contracting node {}\n", node);
        std::unordered_set<EdgeId> obsolete_edges;
        std::vector<std::pair<NodeId, Edge>> new_edges;
        auto [edges, edge_ids] = relaxEdgesWithIds(node);

        for(auto i = 0; i < edges.size(); i++) {
            auto source = edges[i].target;
            // shortest path from neigh to all other neighbors
            for(auto j = 0; j < edges.size(); j++) {
                if(i == j) {
                    // TODO: I don't think we need this check
                    continue;
                }
                auto target = edges[j].target;
                auto res = dijkstra.findRoute(source, target);
                if(res.has_value()) {
                    auto [path, cost] = res.value();
                    // check if shortest path contains node
                    if(path.size() == 3 and path[0] == source and path[1] == node and path[2] == target) {
                        auto wrapped_edge_1 = edge_ids[i]; // this is wrong, we need the inverse edge
                        auto wrapped_edge_2 = edge_ids[j];
                        fmt::print("found shortcut with cost {} and path {} wrapping edges {} and {}\n", cost, path, wrapped_edge_1, wrapped_edge_2);
                        // obsolete_edges.emplace(wrapped_edge_1);
                        obsolete_edges.emplace(wrapped_edge_2);
                        new_edges.emplace_back(
                            source,
                            Edge{target,
                                 cost,
                                 std::pair{-1, wrapped_edge_2}});
                    }
                }
            }
        }
        // TODO: consider alternative to just use amount of neighbors as removed edges
        auto edge_diff = new_edges.size() - obsolete_edges.size();
        newEdgeCandidates.emplace_back(edge_diff, new_edges);
    }
    fmt::print("Done checking for possible shortcuts\n");

    // 3.
    std::sort(newEdgeCandidates.begin(), newEdgeCandidates.end(), [](auto first, auto second) {
        return first.first < second.first;
    });

    // 4.
    std::vector<std::pair<NodeId, Edge>> toInsert;
    for(auto i = 0; i < newEdgeCandidates.size() / 2; i++) {
        auto [_, new_edges] = newEdgeCandidates[i];
        concat(toInsert, new_edges);
    }

    // 5.
    fmt::print("adding {} new shortcuts\n", toInsert.size());
    insertEdges(toInsert);

    // increment level and assign to nodes
    current_level++;
    for(auto node : indep_nodes) {
        levels[node] = current_level;
    }
}

std::vector<NodeId> Graph::independentSet() const
{
    std::vector<bool> visited(size(), false);
    std::vector<NodeId> indepNodes;

    for(auto i = 0; i < size(); i++) {
        if(levels[i] == 0 && !visited[i]) {
            auto edges = relaxEdges(i);
            for(auto e : edges) {
                visited[e.target] = true;
            }
            indepNodes.emplace_back(i);
        }
    }
    return indepNodes;
}

void Graph::insertEdges(std::vector<std::pair<NodeId, Edge>> toInsert)
{
    fmt::print("Updating graph with new edges...\n");
    for(auto [source, new_edge] : toInsert) {
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
        auto edge_id = numEdgesOld + 1;
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