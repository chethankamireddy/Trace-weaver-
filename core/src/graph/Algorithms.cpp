#include "graph/Algorithms.hpp"

#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

namespace traceweaver::graph {

std::vector<std::string> GraphAlgorithms::bfs(const Graph& graph, const std::string& startNodeId) {
    std::vector<std::string> order;
    if (!graph.hasNode(startNodeId)) {
        return order;
    }

    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(startNodeId);
    visited.insert(startNodeId);

    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        order.push_back(current);

        for (const auto& edge : graph.getOutgoingEdges(current)) {
            const std::string& neighbor = edge.getDestinationId();
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    return order;
}

std::vector<std::string> GraphAlgorithms::dfs(const Graph& graph, const std::string& startNodeId) {
    std::vector<std::string> order;
    if (!graph.hasNode(startNodeId)) {
        return order;
    }

    std::unordered_set<std::string> visited;
    std::stack<std::string> st;

    st.push(startNodeId);

    while (!st.empty()) {
        std::string current = st.top();
        st.pop();

        if (visited.contains(current)) {
            continue;
        }

        visited.insert(current);
        order.push_back(current);

        // Iterate in reverse for intuitive left-to-right DFS order
        const auto& edges = graph.getOutgoingEdges(current);
        for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
            const std::string& neighbor = it->getDestinationId();
            if (!visited.contains(neighbor)) {
                st.push(neighbor);
            }
        }
    }

    return order;
}

enum class DfsColor { WHITE, GRAY, BLACK };

static bool dfsCycleHelper(const Graph& graph,
                           const std::string& u,
                           std::unordered_map<std::string, DfsColor>& color) {
    color[u] = DfsColor::GRAY;

    for (const auto& edge : graph.getOutgoingEdges(u)) {
        const std::string& v = edge.getDestinationId();
        if (color[v] == DfsColor::GRAY) {
            return true; // Back-edge detected -> cycle!
        }
        if (color[v] == DfsColor::WHITE) {
            if (dfsCycleHelper(graph, v, color)) {
                return true;
            }
        }
    }

    color[u] = DfsColor::BLACK;
    return false;
}

bool GraphAlgorithms::hasCycle(const Graph& graph) {
    std::unordered_map<std::string, DfsColor> color;
    for (const auto& id : graph.getAllNodeIds()) {
        color[id] = DfsColor::WHITE;
    }

    for (const auto& id : graph.getAllNodeIds()) {
        if (color[id] == DfsColor::WHITE) {
            if (dfsCycleHelper(graph, id, color)) {
                return true;
            }
        }
    }

    return false;
}

static void findCyclesHelper(const Graph& graph,
                             const std::string& u,
                             std::unordered_map<std::string, DfsColor>& color,
                             std::vector<std::string>& currentPath,
                             std::vector<std::vector<std::string>>& allCycles) {
    color[u] = DfsColor::GRAY;
    currentPath.push_back(u);

    for (const auto& edge : graph.getOutgoingEdges(u)) {
        const std::string& v = edge.getDestinationId();
        if (color[v] == DfsColor::GRAY) {
            // Found a back-edge to v, extract the cycle segment from currentPath
            auto startIt = std::find(currentPath.begin(), currentPath.end(), v);
            if (startIt != currentPath.end()) {
                std::vector<std::string> cycle(startIt, currentPath.end());
                cycle.push_back(v); // close the loop for clarity
                allCycles.push_back(cycle);
            }
        } else if (color[v] == DfsColor::WHITE) {
            findCyclesHelper(graph, v, color, currentPath, allCycles);
        }
    }

    currentPath.pop_back();
    color[u] = DfsColor::BLACK;
}

std::vector<std::vector<std::string>> GraphAlgorithms::findCycles(const Graph& graph) {
    std::unordered_map<std::string, DfsColor> color;
    for (const auto& id : graph.getAllNodeIds()) {
        color[id] = DfsColor::WHITE;
    }

    std::vector<std::vector<std::string>> allCycles;
    std::vector<std::string> currentPath;

    for (const auto& id : graph.getAllNodeIds()) {
        if (color[id] == DfsColor::WHITE) {
            findCyclesHelper(graph, id, color, currentPath, allCycles);
        }
    }

    return allCycles;
}

std::optional<std::vector<std::string>> GraphAlgorithms::topologicalSort(const Graph& graph) {
    std::unordered_map<std::string, size_t> inDegree;
    for (const auto& id : graph.getAllNodeIds()) {
        inDegree[id] = 0;
    }

    for (const auto& id : graph.getAllNodeIds()) {
        for (const auto& edge : graph.getOutgoingEdges(id)) {
            ++inDegree[edge.getDestinationId()];
        }
    }

    std::queue<std::string> zeroInDegreeQueue;
    for (const auto& [id, deg] : inDegree) {
        if (deg == 0) {
            zeroInDegreeQueue.push(id);
        }
    }

    std::vector<std::string> sortedOrder;
    sortedOrder.reserve(graph.nodeCount());

    while (!zeroInDegreeQueue.empty()) {
        std::string u = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        sortedOrder.push_back(u);

        for (const auto& edge : graph.getOutgoingEdges(u)) {
            const std::string& v = edge.getDestinationId();
            if (--inDegree[v] == 0) {
                zeroInDegreeQueue.push(v);
            }
        }
    }

    // If topological sort did not include all nodes, there is a cycle
    if (sortedOrder.size() != graph.nodeCount()) {
        return std::nullopt;
    }

    return sortedOrder;
}

bool GraphAlgorithms::isReachable(const Graph& graph, const std::string& sourceId, const std::string& targetId) {
    if (!graph.hasNode(sourceId) || !graph.hasNode(targetId)) {
        return false;
    }
    if (sourceId == targetId) {
        return true;
    }

    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(sourceId);
    visited.insert(sourceId);

    while (!q.empty()) {
        std::string current = q.front();
        q.pop();

        for (const auto& edge : graph.getOutgoingEdges(current)) {
            const std::string& neighbor = edge.getDestinationId();
            if (neighbor == targetId) {
                return true;
            }
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    return false;
}

std::vector<std::string> GraphAlgorithms::findReachableNodes(const Graph& graph, const std::string& startNodeId) {
    return bfs(graph, startNodeId);
}

std::vector<std::string> GraphAlgorithms::findReverseReachableNodes(const Graph& graph, const std::string& startNodeId) {
    std::vector<std::string> reachable;
    if (!graph.hasNode(startNodeId)) {
        return reachable;
    }

    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(startNodeId);
    visited.insert(startNodeId);

    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        reachable.push_back(current);

        // Walk backwards via incoming edges
        for (const auto& edge : graph.getIncomingEdges(current)) {
            const std::string& predecessor = edge.getSourceId();
            if (!visited.contains(predecessor)) {
                visited.insert(predecessor);
                q.push(predecessor);
            }
        }
    }

    return reachable;
}

std::optional<std::vector<std::string>> GraphAlgorithms::shortestPath(
    const Graph& graph, const std::string& startNodeId, const std::string& targetId) {

    if (!graph.hasNode(startNodeId) || !graph.hasNode(targetId)) {
        return std::nullopt;
    }

    if (startNodeId == targetId) {
        return std::vector<std::string>{startNodeId};
    }

    std::unordered_map<std::string, std::string> parent;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(startNodeId);
    visited.insert(startNodeId);

    bool reached = false;
    while (!q.empty() && !reached) {
        std::string current = q.front();
        q.pop();

        for (const auto& edge : graph.getOutgoingEdges(current)) {
            const std::string& neighbor = edge.getDestinationId();
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                parent[neighbor] = current;
                if (neighbor == targetId) {
                    reached = true;
                    break;
                }
                q.push(neighbor);
            }
        }
    }

    if (!reached) {
        return std::nullopt;
    }

    // Reconstruct path
    std::vector<std::string> path;
    std::string curr = targetId;
    while (curr != startNodeId) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(startNodeId);
    std::reverse(path.begin(), path.end());

    return path;
}

} // namespace traceweaver::graph
