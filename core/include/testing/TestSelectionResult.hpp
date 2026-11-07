#pragma once

#include "TestCase.hpp"
#include <vector>
#include <string>
#include <string_view>

namespace traceweaver::testing {

enum class SelectionMode {
    SAFE,       // Comprehensive: all directly or transitively affected tests
    OPTIMIZED   // Ranked and deduplicated minimal test set based on priority and directness
};

[[nodiscard]] constexpr std::string_view selectionModeToString(SelectionMode mode) noexcept {
    switch (mode) {
        case SelectionMode::SAFE:      return "SAFE";
        case SelectionMode::OPTIMIZED: return "OPTIMIZED";
    }
    return "SAFE";
}

struct SelectedTestItem {
    TestCase test;
    std::string selectionReason;
    int relevanceScore{100};
};

struct TestSelectionResult {
    SelectionMode mode{SelectionMode::SAFE};
    size_t totalTestsCount{0};
    size_t selectedTestsCount{0};
    size_t excludedTestsCount{0};
    double selectionPercentage{0.0};
    double estimatedExecutionTimeMs{0.0};
    double totalExecutionTimeSavedMs{0.0};
    std::vector<SelectedTestItem> selectedTests;
    std::vector<TestCase> excludedTests;
};

} // namespace traceweaver::testing
