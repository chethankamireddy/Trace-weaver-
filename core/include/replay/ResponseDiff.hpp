#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace traceweaver::replay {

enum class RegressionType {
    NO_REGRESSION,
    STATUS_REGRESSION,
    STRUCTURAL_REGRESSION,
    VALUE_REGRESSION,
    HEADER_REGRESSION,
    PERFORMANCE_REGRESSION
};

[[nodiscard]] constexpr std::string_view regressionTypeToString(RegressionType reg) noexcept {
    switch (reg) {
        case RegressionType::NO_REGRESSION:          return "NO_REGRESSION";
        case RegressionType::STATUS_REGRESSION:      return "STATUS_REGRESSION";
        case RegressionType::STRUCTURAL_REGRESSION:  return "STRUCTURAL_REGRESSION";
        case RegressionType::VALUE_REGRESSION:       return "VALUE_REGRESSION";
        case RegressionType::HEADER_REGRESSION:      return "HEADER_REGRESSION";
        case RegressionType::PERFORMANCE_REGRESSION: return "PERFORMANCE_REGRESSION";
    }
    return "NO_REGRESSION";
}

struct FieldDifference {
    std::string jsonPointer;
    std::string differenceType; // MISSING_FIELD, EXTRA_FIELD, TYPE_MISMATCH, VALUE_MISMATCH
    std::string expectedValue;
    std::string actualValue;
    bool isIgnored{false};
};

struct ResponseComparisonResult {
    std::string recordingId;
    RegressionType primaryRegression{RegressionType::NO_REGRESSION};
    bool isFailure{false};
    
    int expectedStatus{200};
    int actualStatus{200};
    
    double historicalLatencyMs{0.0};
    double replayLatencyMs{0.0};
    double latencyDifferenceMs{0.0};
    
    std::vector<FieldDifference> fieldDifferences;
    std::string summaryDiff;
};

} // namespace traceweaver::replay
