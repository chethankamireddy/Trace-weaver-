#include "testing/TestSelectionEngine.hpp"
#include <unordered_set>
#include <algorithm>

namespace traceweaver::testing {

TestSelectionResult TestSelectionEngine::selectTests(
    const std::vector<TestCase>& allTests,
    const impact::ImpactAnalysisResult& impactResult,
    SelectionMode mode) {

    TestSelectionResult result;
    result.mode = mode;
    result.totalTestsCount = allTests.size();

    // Map of impacted node ID -> depth and impact level
    std::unordered_set<std::string> impactedOps;
    std::unordered_set<std::string> impactedServices;

    for (const auto& item : impactResult.impactedComponents) {
        if (item.componentType == graph::NodeType::API_OPERATION) {
            impactedOps.insert(item.nodeId);
        } else if (item.componentType == graph::NodeType::SERVICE) {
            impactedServices.insert(item.nodeId);
        }
    }

    double totalPossibleDuration = 0.0;
    for (const auto& test : allTests) {
        totalPossibleDuration += test.executionDurationMs;

        bool isDirectlyImpacted = impactedOps.contains(test.associatedOperationId);
        bool isServiceImpacted = impactedServices.contains(test.associatedServiceId);

        if (isDirectlyImpacted || isServiceImpacted) {
            std::string reason = isDirectlyImpacted 
                ? "Directly covers impacted operation " + test.associatedOperationId 
                : "Covers downstream service " + test.associatedServiceId + " in blast radius";

            int relevance = isDirectlyImpacted ? 100 : 70;

            if (mode == SelectionMode::OPTIMIZED) {
                // In optimized mode, omit low-priority integration/e2e tests unless directly affected
                if (!isDirectlyImpacted && test.priority > 3) {
                    result.excludedTests.push_back(test);
                    continue;
                }
            }

            result.selectedTests.push_back(SelectedTestItem{
                .test = test,
                .selectionReason = reason,
                .relevanceScore = relevance
            });
            result.estimatedExecutionTimeMs += test.executionDurationMs;
        } else {
            result.excludedTests.push_back(test);
        }
    }

    result.selectedTestsCount = result.selectedTests.size();
    result.excludedTestsCount = result.excludedTests.size();
    result.selectionPercentage = (result.totalTestsCount > 0)
        ? (static_cast<double>(result.selectedTestsCount) / static_cast<double>(result.totalTestsCount)) * 100.0
        : 0.0;
    result.totalExecutionTimeSavedMs = totalPossibleDuration - result.estimatedExecutionTimeMs;

    return result;
}

} // namespace traceweaver::testing
