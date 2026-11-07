#pragma once

#include "graph/Node.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace traceweaver::impact {

enum class ImpactLevel {
    DIRECT,
    INDIRECT,
    POTENTIAL,
    UNKNOWN
};

[[nodiscard]] constexpr std::string_view impactLevelToString(ImpactLevel level) noexcept {
    switch (level) {
        case ImpactLevel::DIRECT:    return "DIRECT";
        case ImpactLevel::INDIRECT:  return "INDIRECT";
        case ImpactLevel::POTENTIAL: return "POTENTIAL";
        case ImpactLevel::UNKNOWN:   return "UNKNOWN";
    }
    return "UNKNOWN";
}

struct ImpactedComponent {
    std::string nodeId;
    std::string componentName;
    graph::NodeType componentType;
    ImpactLevel impactLevel;
    size_t depth{0};
    std::vector<std::string> dependencyPath; // Path from change source to this component
    std::string reason;
    std::string relatedChangeId;
};

struct ImpactAnalysisResult {
    std::string analysisRunId;
    std::vector<ImpactedComponent> impactedComponents;
    size_t impactedServiceCount{0};
    size_t impactedOperationCount{0};
    size_t impactedClientCount{0};
    size_t impactedFieldCount{0};
    size_t maxDependencyDepth{0};
    bool isGraphComplete{true};
    std::vector<std::string> warnings;
};

} // namespace traceweaver::impact
