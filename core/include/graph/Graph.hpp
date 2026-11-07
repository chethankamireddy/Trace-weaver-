#pragma once

#include "Node.hpp"
#include "Edge.hpp"

#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include <string>
#include <stdexcept>

namespace traceweaver::graph {

class Graph {
public:
    Graph() = default;
    ~Graph() = default;

    // Movable, copyable
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;
    Graph(Graph&&) noexcept = default;
    Graph& operator=(Graph&&) noexcept = default;

    // Node management
    bool addNode(const Node& node);
    bool addNode(Node&& node);
    bool removeNode(const std::string& nodeId);
    [[nodiscard]] bool hasNode(const std::string& nodeId) const noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<const Node>> getNode(const std::string& nodeId) const noexcept;
    [[nodiscard]] std::optional<std::reference_wrapper<Node>> getNodeMut(const std::string& nodeId) noexcept;

    // Edge management
    bool addEdge(const Edge& edge);
    bool removeEdge(const std::string& sourceId, const std::string& destinationId, 
                    std::optional<RelationshipType> rel = std::nullopt);
    [[nodiscard]] bool hasEdge(const std::string& sourceId, const std::string& destinationId,
                               std::optional<RelationshipType> rel = std::nullopt) const noexcept;

    // Neighbor queries
    [[nodiscard]] const std::vector<Edge>& getOutgoingEdges(const std::string& nodeId) const;
    [[nodiscard]] const std::vector<Edge>& getIncomingEdges(const std::string& nodeId) const;
    [[nodiscard]] std::vector<std::string> getSuccessors(const std::string& nodeId) const;
    [[nodiscard]] std::vector<std::string> getPredecessors(const std::string& nodeId) const;

    // Graph statistics and inspection
    [[nodiscard]] size_t nodeCount() const noexcept { return nodes_.size(); }
    [[nodiscard]] size_t edgeCount() const noexcept { return edgeCount_; }
    [[nodiscard]] std::vector<std::string> getAllNodeIds() const;
    [[nodiscard]] const std::unordered_map<std::string, Node>& getNodes() const noexcept { return nodes_; }

    void clear() noexcept;

private:
    std::unordered_map<std::string, Node> nodes_;
    std::unordered_map<std::string, std::vector<Edge>> outgoingEdges_;
    std::unordered_map<std::string, std::vector<Edge>> incomingEdges_;
    size_t edgeCount_{0};

    // Static empty vector for safe references
    static const std::vector<Edge> emptyEdgeList_;
};

} // namespace traceweaver::graph
