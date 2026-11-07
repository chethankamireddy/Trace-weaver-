#pragma once

#include "graph/Graph.hpp"
#include "diff/DiffReport.hpp"
#include "ImpactedComponent.hpp"

namespace traceweaver::impact {

class ImpactAnalysisEngine {
public:
    ImpactAnalysisEngine() = default;

    // Analyzes a set of contract changes across the dependency graph
    [[nodiscard]] ImpactAnalysisResult analyzeImpact(
        const graph::Graph& graph,
        const diff::DiffReport& diffReport);

private:
    void traverseImpact(
        const graph::Graph& graph,
        const std::string& startNodeId,
        const std::string& changeId,
        ImpactAnalysisResult& result,
        std::unordered_set<std::string>& visited);
};

} // namespace traceweaver::impact
