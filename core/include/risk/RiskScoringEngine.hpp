#pragma once

#include "RiskAssessment.hpp"
#include "diff/DiffReport.hpp"
#include "impact/ImpactedComponent.hpp"

namespace traceweaver::risk {

enum class ServiceCriticality {
    LOW,        // Factor 0.5
    MEDIUM,     // Factor 1.0
    HIGH,       // Factor 1.5
    CRITICAL    // Factor 2.0
};

class RiskScoringEngine {
public:
    RiskScoringEngine() = default;

    // Computes explainable risk assessment based on diff, impact traversal, service criticality, and replay test failures
    [[nodiscard]] RiskAssessment calculateRisk(
        const diff::DiffReport& diffReport,
        const impact::ImpactAnalysisResult& impactResult,
        ServiceCriticality criticality = ServiceCriticality::MEDIUM,
        size_t failedReplayTests = 0,
        size_t totalReplayTests = 0);
};

} // namespace traceweaver::risk
