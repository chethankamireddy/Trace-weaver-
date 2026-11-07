#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace traceweaver::risk {

enum class RiskClassification {
    LOW,        // 0 - 29
    MEDIUM,     // 30 - 59
    HIGH,       // 60 - 84
    CRITICAL    // 85 - 100
};

[[nodiscard]] constexpr std::string_view riskClassificationToString(RiskClassification classification) noexcept {
    switch (classification) {
        case RiskClassification::LOW:      return "LOW";
        case RiskClassification::MEDIUM:   return "MEDIUM";
        case RiskClassification::HIGH:     return "HIGH";
        case RiskClassification::CRITICAL: return "CRITICAL";
    }
    return "LOW";
}

struct RiskFactorBreakdown {
    double changeSeverityScore{0.0};      // 0 - 30 points
    double impactBreadthScore{0.0};       // 0 - 25 points
    double dependencyDepthScore{0.0};     // 0 - 15 points
    double serviceCriticalityScore{0.0};  // 0 - 15 points
    double regressionFailureScore{0.0};   // 0 - 15 points
    
    std::string severityExplanation;
    std::string breadthExplanation;
    std::string depthExplanation;
    std::string criticalityExplanation;
    std::string regressionExplanation;
};

struct RiskAssessment {
    double overallScore{0.0}; // 0 to 100
    RiskClassification classification{RiskClassification::LOW};
    RiskFactorBreakdown breakdown;
    std::vector<std::string> recommendedActions;
};

} // namespace traceweaver::risk
