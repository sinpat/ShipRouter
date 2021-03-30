#pragma once

#include <Constants.hpp>
#include <functional>

template<class T>
using Ref = std::reference_wrapper<T>;

template<class T>
using CRef = std::reference_wrapper<const T>;

using Tags = std::unordered_map<std::string, std::string>;


template<class Head0, class Head1, class... Tail>
constexpr auto concat(Head0&& head0, Head1&& head1, Tail&&... tail) noexcept
{
    if constexpr(sizeof...(tail) == 0) {
        head0.insert(std::end(head0),
                     std::begin(head1),
                     std::end(head1));
        return std::forward<Head0>(head0);
    } else {
        return concat(
            concat(std::forward<Head0>(head0),
                   std::forward<Head1>(head1)),
            std::forward<Tail>(tail)...);
    }
}


using Distance = std::uint64_t;
using NodeId = std::uint64_t;
using EdgeId = std::uint64_t;
using Level = std::uint64_t;
using Path = std::vector<NodeId>;

struct Edge
{
    Edge(NodeId target, Distance dist, std::optional<std::pair<EdgeId, EdgeId>> wrapped_edges)
        : target(target),
          dist(dist),
          wrapped_edges(wrapped_edges)
    {
    }
    NodeId target;
    Distance dist;
    std::optional<std::pair<EdgeId, EdgeId>> wrapped_edges;
};

constexpr static inline auto UNREACHABLE = std::numeric_limits<Distance>::max();
constexpr static inline auto NON_EXISTENT = std::numeric_limits<NodeId>::max();
