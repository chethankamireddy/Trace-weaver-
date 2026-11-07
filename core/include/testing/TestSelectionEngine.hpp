#pragma once

#include "TestCase.hpp"
#include "TestSelectionResult.hpp"
#include "impact/ImpactedComponent.hpp"
#include "graph/Graph.hpp"
#include <vector>

namespace traceweaver::testing {

class TestSelectionEngine {
public:
    TestSelectionEngine() = default;

    [[nodiscard]] TestSelectionResult selectTests(
        const std::vector<TestCase>& allTests,
        const impact::ImpactAnalysisResult& impactResult,
        SelectionMode mode = SelectionMode::SAFE);
};

} // namespace traceweaver::testing
