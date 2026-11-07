#include "risk/RiskScoringEngine.hpp"
#include <algorithm>
#include <sstream>

namespace traceweaver::risk {

RiskAssessment RiskScoringEngine::calculateRisk(
    const diff::DiffReport& diffReport,
    const impact::ImpactAnalysisResult& impactResult,
    ServiceCriticality criticality,
    size_t failedReplayTests,
    size_t totalReplayTests) {

    RiskAssessment assessment;
    auto& b = assessment.breakdown;

    // 1. Change Severity Component (Max 30 pts)
    // Critical breaking = 15 pts each, High = 8 pts, Medium = 4 pts (capped at 30)
    double severityAccumulator = 0.0;
    for (const auto& chg : diffReport.changes) {
        if (chg.compatibility == diff::Compatibility::BREAKING) {
            if (chg.severity == diff::ChangeSeverity::CRITICAL) severityAccumulator += 15.0;
            else if (chg.severity == diff::ChangeSeverity::HIGH) severityAccumulator += 10.0;
            else severityAccumulator += 5.0;
        } else if (chg.compatibility == diff::Compatibility::POTENTIALLY_BREAKING) {
            severityAccumulator += 4.0;
        }
    }
    b.changeSeverityScore = std::min(30.0, severityAccumulator);
    b.severityExplanation = std::to_string(diffReport.breakingCount) + " breaking changes detected (contributing " + 
                            std::to_string(static_cast<int>(b.changeSeverityScore)) + "/30 pts)";

    // 2. Impact Breadth Component (Max 25 pts)
    // Services (5 pts each), Clients (5 pts each), Operations (2 pts each)
    double breadthAccumulator = (impactResult.impactedServiceCount * 6.0) +
                                (impactResult.impactedClientCount * 5.0) +
                                (impactResult.impactedOperationCount * 2.0);
    b.impactBreadthScore = std::min(25.0, breadthAccumulator);
    b.breadthExplanation = std::to_string(impactResult.impactedServiceCount) + " services and " +
                           std::to_string(impactResult.impactedClientCount) + " clients impacted (" +
                           std::to_string(static_cast<int>(b.impactBreadthScore)) + "/25 pts)";

    // 3. Dependency Depth Component (Max 15 pts)
    // Depth 1 = 5 pts, Depth 2 = 10 pts, Depth 3+ = 15 pts
    if (impactResult.maxDependencyDepth == 0) b.dependencyDepthScore = 0.0;
    else if (impactResult.maxDependencyDepth == 1) b.dependencyDepthScore = 5.0;
    else if (impactResult.maxDependencyDepth == 2) b.dependencyDepthScore = 10.0;
    else b.dependencyDepthScore = 15.0;
    b.depthExplanation = "Maximum blast radius traversal depth: " + 
                         std::to_string(impactResult.maxDependencyDepth) + " hops (" +
                         std::to_string(static_cast<int>(b.dependencyDepthScore)) + "/15 pts)";

    // 4. Service Criticality (Max 15 pts)
    switch (criticality) {
        case ServiceCriticality::LOW:
            b.serviceCriticalityScore = 3.0;
            b.criticalityExplanation = "Service criticality rated LOW (3/15 pts)";
            break;
        case ServiceCriticality::MEDIUM:
            b.serviceCriticalityScore = 7.0;
            b.criticalityExplanation = "Service criticality rated MEDIUM (7/15 pts)";
            break;
        case ServiceCriticality::HIGH:
            b.serviceCriticalityScore = 11.0;
            b.criticalityExplanation = "Service criticality rated HIGH (11/15 pts)";
            break;
        case ServiceCriticality::CRITICAL:
            b.serviceCriticalityScore = 15.0;
            b.criticalityExplanation = "Core tier-1 mission critical service (15/15 pts)";
            break;
    }

    // 5. Confirmed Regression Failures (Max 15 pts)
    if (totalReplayTests > 0) {
        double failRatio = static_cast<double>(failedReplayTests) / static_cast<double>(totalReplayTests);
        b.regressionFailureScore = std::min(15.0, failRatio * 15.0);
        b.regressionExplanation = std::to_string(failedReplayTests) + "/" + 
                                  std::to_string(totalReplayTests) + " replayed requests failed regression check";
    } else {
        b.regressionFailureScore = 0.0;
        b.regressionExplanation = "No replay test suite executed yet (0 pts)";
    }

    // Overall Score
    assessment.overallScore = b.changeSeverityScore + b.impactBreadthScore + 
                              b.dependencyDepthScore + b.serviceCriticalityScore + 
                              b.regressionFailureScore;
    assessment.overallScore = std::clamp(assessment.overallScore, 0.0, 100.0);

    // Classification
    if (assessment.overallScore >= 85.0) {
        assessment.classification = RiskClassification::CRITICAL;
        assessment.recommendedActions.push_back("BLOCK automatic deployment: Critical contract violations across core services");
        assessment.recommendedActions.push_back("Notify downstream service owners immediately for consumer contract deprecation");
    } else if (assessment.overallScore >= 60.0) {
        assessment.classification = RiskClassification::HIGH;
        assessment.recommendedActions.push_back("Require dual approval from impacted service maintainers");
        assessment.recommendedActions.push_back("Execute full regression replay before staging deployment");
    } else if (assessment.overallScore >= 30.0) {
        assessment.classification = RiskClassification::MEDIUM;
        assessment.recommendedActions.push_back("Run targeted selective regression tests for impacted endpoints");
    } else {
        assessment.classification = RiskClassification::LOW;
        assessment.recommendedActions.push_back("Changes are backward-compatible; standard CI workflow may proceed");
    }

    return assessment;
}

} // namespace traceweaver::risk
