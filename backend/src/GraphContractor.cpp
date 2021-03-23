#include <CHGraph.hpp>
#include <GraphContractor.hpp>

GraphContractor::GraphContractor(Graph g_in)
    : g(std::move(g_in)),
      numNodes(g.size()),
      level(numNodes, 0),
      edges(g.edges())
{}

CHGraph GraphContractor::contract() const
{
    return CHGraph();
}


std::vector<NodeId> GraphContractor::independentSet() const
{
    std::vector<bool> visited(numNodes, false);
    std::vector<NodeId> indepNodes;

    for(auto i = 0; i < numNodes; i++) {
        if(level[i] == 0 && !visited[i]) {
            auto neighbors = g.getNeigboursOf(i);
            for(auto [neigh, _] : neighbors) {
                visited[neigh] = true;
            }
            indepNodes.emplace_back(i);
        }
    }
    return indepNodes;
}
