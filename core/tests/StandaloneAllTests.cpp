#include "graph/Graph.hpp"
#include "graph/Algorithms.hpp"
#include "parser/OpenAPIParser.hpp"
#include "diff/ContractDiffEngine.hpp"
#include "impact/ImpactAnalysisEngine.hpp"
#include "risk/RiskScoringEngine.hpp"
#include "testing/TestSelectionEngine.hpp"
#include "replay/RequestReplayEngine.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace traceweaver;

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(fn) \
    do { \
        std::cout << "[RUN] " << #fn << "...\n"; \
        if (fn()) { \
            std::cout << "  [PASS] " << #fn << "\n"; \
            ++g_testsPassed; \
        } else { \
            ++g_testsFailed; \
        } \
    } while(0)

bool testGraphCycleAndTopologicalSort() {
    graph::Graph g;
    g.addNode(graph::Node("A", graph::NodeType::SERVICE, "Auth Service"));
    g.addNode(graph::Node("B", graph::NodeType::SERVICE, "User Service"));
    g.addNode(graph::Node("C", graph::NodeType::SERVICE, "Order Service"));

    g.addEdge(graph::Edge("A", "B", graph::RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(graph::Edge("B", "C", graph::RelationshipType::SERVICE_DEPENDS_ON_SERVICE));

    TEST_ASSERT(!graph::GraphAlgorithms::hasCycle(g), "DAG should not have cycles");
    auto topo = graph::GraphAlgorithms::topologicalSort(g);
    TEST_ASSERT(topo.has_value(), "Topological sort should succeed for DAG");
    TEST_ASSERT(topo->size() == 3, "All 3 nodes must be present in topological order");

    // Add back-edge C -> A
    g.addEdge(graph::Edge("C", "A", graph::RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    TEST_ASSERT(graph::GraphAlgorithms::hasCycle(g), "Cycle must be detected after back-edge");
    TEST_ASSERT(!graph::GraphAlgorithms::topologicalSort(g).has_value(), "Topological sort must fail on cycle");

    return true;
}

bool testContractDiffEngine() {
    diff::ContractDiffEngine diffEngine;

    parser::ApiSpecification oldSpec;
    oldSpec.version = "1.0.0";
    parser::ApiOperation oldOp;
    oldOp.operationId = "getUserById";
    oldOp.method = "GET";
    oldOp.path = "/api/v1/users/{id}";
    
    auto userSchemaOld = std::make_shared<parser::SchemaObject>();
    userSchemaOld->name = "User";
    userSchemaOld->properties["id"] = parser::SchemaField{.name = "id", .type = parser::SchemaType::INTEGER};
    userSchemaOld->properties["name"] = parser::SchemaField{.name = "name", .type = parser::SchemaType::STRING};
    
    parser::ResponseDefinition resp200Old;
    resp200Old.statusCode = 200;
    resp200Old.schema = userSchemaOld;
    oldOp.responses[200] = resp200Old;
    oldSpec.operations["GET:/api/v1/users/{id}"] = oldOp;

    // New version: removed 'name', added optional 'fullName'
    parser::ApiSpecification newSpec;
    newSpec.version = "2.0.0";
    parser::ApiOperation newOp = oldOp;
    auto userSchemaNew = std::make_shared<parser::SchemaObject>();
    userSchemaNew->name = "User";
    userSchemaNew->properties["id"] = parser::SchemaField{.name = "id", .type = parser::SchemaType::INTEGER};
    userSchemaNew->properties["fullName"] = parser::SchemaField{.name = "fullName", .type = parser::SchemaType::STRING};
    
    parser::ResponseDefinition resp200New;
    resp200New.statusCode = 200;
    resp200New.schema = userSchemaNew;
    newOp.responses[200] = resp200New;
    newSpec.operations["GET:/api/v1/users/{id}"] = newOp;

    auto report = diffEngine.compare(oldSpec, newSpec);
    TEST_ASSERT(report.hasBreakingChanges(), "Should detect breaking changes due to removed 'name' field");
    TEST_ASSERT(report.breakingCount == 1, "Exactly 1 breaking change expected");
    TEST_ASSERT(report.nonBreakingCount == 1, "Exactly 1 non-breaking change expected (added 'fullName')");
    TEST_ASSERT(report.changes[0].compatibility == diff::Compatibility::BREAKING, "First change is BREAKING");

    return true;
}

bool testImpactAnalysisEngine() {
    // Graph: User Service -> Order Service -> Notification Service
    // Change is on User Service "getUserById"
    graph::Graph g;
    g.addNode(graph::Node("user_service", graph::NodeType::SERVICE, "User Service"));
    g.addNode(graph::Node("order_service", graph::NodeType::SERVICE, "Order Service"));
    g.addNode(graph::Node("notif_service", graph::NodeType::SERVICE, "Notification Service"));
    g.addNode(graph::Node("mobile_app", graph::NodeType::CLIENT, "iOS App"));

    // Directed edges from callers to providers
    g.addEdge(graph::Edge("order_service", "user_service", graph::RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(graph::Edge("notif_service", "order_service", graph::RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(graph::Edge("mobile_app", "user_service", graph::RelationshipType::CLIENT_CALLS_OPERATION));

    diff::DiffReport report;
    report.newVersion = "2.0.0";
    report.changes.push_back(diff::ApiChange{
        .changeId = "chg_1",
        .type = diff::ChangeType::RESPONSE_FIELD_REMOVED,
        .compatibility = diff::Compatibility::BREAKING,
        .severity = diff::ChangeSeverity::CRITICAL,
        .operationId = "user_service"
    });

    impact::ImpactAnalysisEngine impactEngine;
    auto impactResult = impactEngine.analyzeImpact(g, report);

    TEST_ASSERT(impactResult.impactedServiceCount == 2, "2 services should be in blast radius (order and notif)");
    TEST_ASSERT(impactResult.impactedClientCount == 1, "1 client should be in blast radius (mobile_app)");
    TEST_ASSERT(impactResult.maxDependencyDepth == 2, "Max depth should be 2 hops (notif -> order -> user)");

    return true;
}

bool testRiskScoringEngine() {
    diff::DiffReport report;
    report.breakingCount = 2;
    report.changes.push_back(diff::ApiChange{
        .compatibility = diff::Compatibility::BREAKING,
        .severity = diff::ChangeSeverity::CRITICAL
    });
    report.changes.push_back(diff::ApiChange{
        .compatibility = diff::Compatibility::BREAKING,
        .severity = diff::ChangeSeverity::HIGH
    });

    impact::ImpactAnalysisResult impact;
    impact.impactedServiceCount = 3;
    impact.impactedClientCount = 2;
    impact.impactedOperationCount = 4;
    impact.maxDependencyDepth = 3;

    risk::RiskScoringEngine riskEngine;
    auto assessment = riskEngine.calculateRisk(report, impact, risk::ServiceCriticality::CRITICAL, 2, 10);

    TEST_ASSERT(assessment.overallScore >= 75.0, "High severity + high breadth + depth + critical tier must produce score >= 75");
    TEST_ASSERT(assessment.classification == risk::RiskClassification::HIGH || 
                assessment.classification == risk::RiskClassification::CRITICAL, "Must classify as HIGH or CRITICAL risk");
    TEST_ASSERT(!assessment.recommendedActions.empty(), "Must provide actionable recommendations");

    return true;
}

bool testTestSelectionEngine() {
    testing::TestSelectionEngine selector;

    std::vector<testing::TestCase> tests = {
        {.testId = "T1", .name = "User Auth Test", .associatedServiceId = "user_service", .associatedOperationId = "op_login", .priority = 1, .executionDurationMs = 100.0},
        {.testId = "T2", .name = "Order Checkout Test", .associatedServiceId = "order_service", .associatedOperationId = "op_checkout", .priority = 2, .executionDurationMs = 250.0},
        {.testId = "T3", .name = "Inventory Sync Test", .associatedServiceId = "inventory_service", .associatedOperationId = "op_sync", .priority = 5, .executionDurationMs = 400.0}
    };

    impact::ImpactAnalysisResult impact;
    impact.impactedComponents.push_back(impact::ImpactedComponent{
        .nodeId = "order_service",
        .componentType = graph::NodeType::SERVICE
    });

    // Safe mode
    auto safeResult = selector.selectTests(tests, impact, testing::SelectionMode::SAFE);
    TEST_ASSERT(safeResult.selectedTestsCount == 1, "Only order_service test should be selected");
    TEST_ASSERT(safeResult.selectedTests[0].test.testId == "T2", "T2 must be selected");
    TEST_ASSERT(safeResult.totalExecutionTimeSavedMs == 500.0, "Time saved should be 100 + 400 = 500ms");

    return true;
}

bool testReplayRedactorAndComparator() {
    replay::RequestRecording rec;
    rec.recordingId = "rec_001";
    rec.httpMethod = "GET";
    rec.urlPath = "/api/v1/profile";
    rec.requestHeaders["Authorization"] = "Bearer eyJhbGciOi...";
    rec.requestHeaders["Accept"] = "application/json";
    rec.responseStatus = 200;
    rec.responseBody = "{\"id\": 1, \"name\": \"Alex\"}";
    rec.latencyMs = 40.0;

    replay::DataRedactor::redactRecording(rec);
    TEST_ASSERT(rec.requestHeaders["Authorization"] == "[REDACTED]", "Auth token must be redacted");
    TEST_ASSERT(rec.requestHeaders["Accept"] == "application/json", "Non-sensitive header preserved");

    replay::ResponseComparator comparator;
    // Status mismatch regression
    auto diffResult = comparator.compareResponses(rec, 500, {}, "Internal Server Error", 50.0);
    TEST_ASSERT(diffResult.isFailure, "Status code 500 vs 200 must be marked failure");
    TEST_ASSERT(diffResult.primaryRegression == replay::RegressionType::STATUS_REGRESSION, "Must be STATUS_REGRESSION");

    return true;
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "TraceWeaver All-Engine Integration Test Suite (Phases 1-6)\n";
    std::cout << "========================================================\n";

    RUN_TEST(testGraphCycleAndTopologicalSort);
    RUN_TEST(testContractDiffEngine);
    RUN_TEST(testImpactAnalysisEngine);
    RUN_TEST(testRiskScoringEngine);
    RUN_TEST(testTestSelectionEngine);
    RUN_TEST(testReplayRedactorAndComparator);

    std::cout << "========================================================\n";
    std::cout << "Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed.\n";
    std::cout << "========================================================\n";

    return (g_testsFailed == 0) ? 0 : 1;
}
