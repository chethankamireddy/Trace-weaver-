#pragma once

#include "Graph.hpp"
#include <vector>
#include <string>
#include <optional>

namespace traceweaver::graph {

class GraphAlgorithms {
public:
    // Breadth-First Search traversal starting from startNodeId
    [[nodiscard]] static std::vector<std::string> bfs(const Graph& graph, const std::string& startNodeId);

    // Depth-First Search traversal starting from startNodeId
    [[nodiscard]] static std::vector<std::string> dfs(const Graph& graph, const std::string& startNodeId);

    // Cycle detection for directed graph (returns true if at least one cycle exists)
    [[nodiscard]] static bool hasCycle(const Graph& graph);

    // Returns detected simple cycle paths in the graph
    [[nodiscard]] static std::vector<std::vector<std::string>> findCycles(const Graph& graph);

    // Topological sorting using Kahn's algorithm (indegree reduction).
    // Returns an empty vector if graph contains cycles (not a DAG).
    [[nodiscard]] static std::optional<std::vector<std::string>> topologicalSort(const Graph& graph);

    // Reachability query: can targetId be reached from sourceId?
    [[nodiscard]] static bool isReachable(const Graph& graph, const std::string& sourceId, const std::string& targetId);

    // Forward reachability: all downstream nodes reachable from startNodeId
    [[nodiscard]] static std::vector<std::string> findReachableNodes(const Graph& graph, const std::string& startNodeId);

    // Reverse reachability: all upstream / dependent nodes that can reach startNodeId
    // Essential for API change impact analysis (who depends on this modified component?)
    [[nodiscard]] static std::vector<std::string> findReverseReachableNodes(const Graph& graph, const std::string& startNodeId);

    // Shortest dependency path from startNodeId to targetId (returns list of node IDs along path)
    [[nodiscard]] static std::optional<std::vector<std::string>> shortestPath(
        const Graph& graph, const std::string& startNodeId, const std::string& targetId);
};

} // namespace traceweaver::graph
