#include "impact/ImpactAnalysisEngine.hpp"
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace traceweaver::impact {

ImpactAnalysisResult ImpactAnalysisEngine::analyzeImpact(
    const graph::Graph& graph,
    const diff::DiffReport& diffReport) {

    ImpactAnalysisResult result;
    result.analysisRunId = "run_" + diffReport.newVersion;
    
    std::unordered_set<std::string> globalImpactedNodeIds;
    std::unordered_set<std::string> impactedServices;
    std::unordered_set<std::string> impactedOperations;
    std::unordered_set<std::string> impactedClients;

    for (const auto& change : diffReport.changes) {
        // Only breaking or potentially breaking changes propagate blast radius
        if (change.compatibility == diff::Compatibility::NON_BREAKING) {
            continue;
        }

        // Target starting node: e.g. "op:<operationId>" or "service:<serviceId>"
        std::string startNodeId = change.operationId;
        if (!graph.hasNode(startNodeId)) {
            // Check if node exists by operation name or prefix
            for (const auto& id : graph.getAllNodeIds()) {
                if (id.find(change.operationId) != std::string::npos) {
                    startNodeId = id;
                    break;
                }
            }
        }

        if (!graph.hasNode(startNodeId)) {
            result.warnings.push_back("No graph node found for operation ID: " + change.operationId);
            continue;
        }

        // BFS upstream through incoming edges (reverse traversal)
        std::unordered_set<std::string> visited;
        std::queue<std::pair<std::string, size_t>> q;
        std::unordered_map<std::string, std::string> parentMap;

        q.push({startNodeId, 0});
        visited.insert(startNodeId);

        while (!q.empty()) {
            auto [currentId, depth] = q.front();
            q.pop();

            if (depth > 0 && !globalImpactedNodeIds.contains(currentId)) {
                globalImpactedNodeIds.insert(currentId);
                
                auto nodeOpt = graph.getNode(currentId);
                if (nodeOpt.has_value()) {
                    const auto& node = nodeOpt->get();
                    
                    // Reconstruct path from start
                    std::vector<std::string> path;
                    std::string p = currentId;
                    while (p != startNodeId && !p.empty()) {
                        path.push_back(p);
                        p = parentMap[p];
                    }
                    path.push_back(startNodeId);
                    std::reverse(path.begin(), path.end());

                    ImpactLevel level = (depth == 1) ? ImpactLevel::DIRECT : ImpactLevel::INDIRECT;

                    result.impactedComponents.push_back(ImpactedComponent{
                        .nodeId = currentId,
                        .componentName = node.getName(),
                        .componentType = node.getType(),
                        .impactLevel = level,
                        .depth = depth,
                        .dependencyPath = path,
                        .reason = (depth == 1) 
                            ? "Direct downstream dependency calling modified component" 
                            : "Transitive downstream dependency impacted via dependency path",
                        .relatedChangeId = change.changeId
                    });

                    result.maxDependencyDepth = std::max(result.maxDependencyDepth, depth);

                    if (node.getType() == graph::NodeType::SERVICE) {
                        impactedServices.insert(currentId);
                    } else if (node.getType() == graph::NodeType::API_OPERATION) {
                        impactedOperations.insert(currentId);
                    } else if (node.getType() == graph::NodeType::CLIENT) {
                        impactedClients.insert(currentId);
                    }
                }
            }

            // Upstream traversal
            for (const auto& edge : graph.getIncomingEdges(currentId)) {
                const std::string& upstream = edge.getSourceId();
                if (!visited.contains(upstream)) {
                    visited.insert(upstream);
                    parentMap[upstream] = currentId;
                    q.push({upstream, depth + 1});
                }
            }
        }
    }

    result.impactedServiceCount = impactedServices.size();
    result.impactedOperationCount = impactedOperations.size();
    result.impactedClientCount = impactedClients.size();

    return result;
}

} // namespace traceweaver::impact
