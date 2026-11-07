#include "graph/Graph.hpp"
#include <algorithm>

namespace traceweaver::graph {

const std::vector<Edge> Graph::emptyEdgeList_{};

bool Graph::addNode(const Node& node) {
    if (nodes_.contains(node.getId())) {
        return false;
    }
    nodes_.emplace(node.getId(), node);
    outgoingEdges_.emplace(node.getId(), std::vector<Edge>{});
    incomingEdges_.emplace(node.getId(), std::vector<Edge>{});
    return true;
}

bool Graph::addNode(Node&& node) {
    const std::string id = node.getId();
    if (nodes_.contains(id)) {
        return false;
    }
    nodes_.emplace(id, std::move(node));
    outgoingEdges_.emplace(id, std::vector<Edge>{});
    incomingEdges_.emplace(id, std::vector<Edge>{});
    return true;
}

bool Graph::removeNode(const std::string& nodeId) {
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        return false;
    }

    // Remove all outgoing edges from this node
    if (auto outIt = outgoingEdges_.find(nodeId); outIt != outgoingEdges_.end()) {
        for (const auto& edge : outIt->second) {
            auto& inc = incomingEdges_[edge.getDestinationId()];
            std::erase_if(inc, [&](const Edge& e) { return e.getSourceId() == nodeId; });
            --edgeCount_;
        }
        outgoingEdges_.erase(outIt);
    }

    // Remove all incoming edges to this node
    if (auto incIt = incomingEdges_.find(nodeId); incIt != incomingEdges_.end()) {
        for (const auto& edge : incIt->second) {
            auto& out = outgoingEdges_[edge.getSourceId()];
            std::erase_if(out, [&](const Edge& e) { return e.getDestinationId() == nodeId; });
            --edgeCount_;
        }
        incomingEdges_.erase(incIt);
    }

    nodes_.erase(it);
    return true;
}

bool Graph::hasNode(const std::string& nodeId) const noexcept {
    return nodes_.contains(nodeId);
}

std::optional<std::reference_wrapper<const Node>> Graph::getNode(const std::string& nodeId) const noexcept {
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        return std::nullopt;
    }
    return std::cref(it->second);
}

std::optional<std::reference_wrapper<Node>> Graph::getNodeMut(const std::string& nodeId) noexcept {
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        return std::nullopt;
    }
    return std::ref(it->second);
}

bool Graph::addEdge(const Edge& edge) {
    // Both endpoints must exist
    if (!nodes_.contains(edge.getSourceId()) || !nodes_.contains(edge.getDestinationId())) {
        return false;
    }

    // Prevent duplicate identical edges
    auto& out = outgoingEdges_[edge.getSourceId()];
    for (const auto& existing : out) {
        if (existing == edge) {
            return false;
        }
    }

    out.push_back(edge);
    incomingEdges_[edge.getDestinationId()].push_back(edge);
    ++edgeCount_;
    return true;
}

bool Graph::removeEdge(const std::string& sourceId, const std::string& destinationId,
                       std::optional<RelationshipType> rel) {
    auto outIt = outgoingEdges_.find(sourceId);
    if (outIt == outgoingEdges_.end()) {
        return false;
    }

    auto matchPredicate = [&](const Edge& e) {
        return e.getDestinationId() == destinationId && (!rel.has_value() || e.getRelationship() == *rel);
    };

    auto originalSize = outIt->second.size();
    std::erase_if(outIt->second, matchPredicate);
    auto removedCount = originalSize - outIt->second.size();

    if (removedCount > 0) {
        auto incIt = incomingEdges_.find(destinationId);
        if (incIt != incomingEdges_.end()) {
            auto incPredicate = [&](const Edge& e) {
                return e.getSourceId() == sourceId && (!rel.has_value() || e.getRelationship() == *rel);
            };
            std::erase_if(incIt->second, incPredicate);
        }
        edgeCount_ -= removedCount;
        return true;
    }

    return false;
}

bool Graph::hasEdge(const std::string& sourceId, const std::string& destinationId,
                    std::optional<RelationshipType> rel) const noexcept {
    auto it = outgoingEdges_.find(sourceId);
    if (it == outgoingEdges_.end()) {
        return false;
    }
    for (const auto& edge : it->second) {
        if (edge.getDestinationId() == destinationId) {
            if (!rel.has_value() || edge.getRelationship() == *rel) {
                return true;
            }
        }
    }
    return false;
}

const std::vector<Edge>& Graph::getOutgoingEdges(const std::string& nodeId) const {
    auto it = outgoingEdges_.find(nodeId);
    if (it == outgoingEdges_.end()) {
        return emptyEdgeList_;
    }
    return it->second;
}

const std::vector<Edge>& Graph::getIncomingEdges(const std::string& nodeId) const {
    auto it = incomingEdges_.find(nodeId);
    if (it == incomingEdges_.end()) {
        return emptyEdgeList_;
    }
    return it->second;
}

std::vector<std::string> Graph::getSuccessors(const std::string& nodeId) const {
    std::vector<std::string> successors;
    for (const auto& edge : getOutgoingEdges(nodeId)) {
        successors.push_back(edge.getDestinationId());
    }
    return successors;
}

std::vector<std::string> Graph::getPredecessors(const std::string& nodeId) const {
    std::vector<std::string> predecessors;
    for (const auto& edge : getIncomingEdges(nodeId)) {
        predecessors.push_back(edge.getSourceId());
    }
    return predecessors;
}

std::vector<std::string> Graph::getAllNodeIds() const {
    std::vector<std::string> ids;
    ids.reserve(nodes_.size());
    for (const auto& [id, _] : nodes_) {
        ids.push_back(id);
    }
    return ids;
}

void Graph::clear() noexcept {
    nodes_.clear();
    outgoingEdges_.clear();
    incomingEdges_.clear();
    edgeCount_ = 0;
}

} // namespace traceweaver::graph
