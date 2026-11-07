#include "graph/Graph.hpp"
#include "graph/Algorithms.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace traceweaver::graph;

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

bool testGraphCreationAndBasicOps() {
    Graph g;
    TEST_ASSERT(g.nodeCount() == 0, "Initial node count should be 0");
    TEST_ASSERT(g.edgeCount() == 0, "Initial edge count should be 0");

    Node n1("user_service", NodeType::SERVICE, "User Service");
    Node n2("order_service", NodeType::SERVICE, "Order Service");

    TEST_ASSERT(g.addNode(n1), "Should successfully add node 1");
    TEST_ASSERT(g.addNode(n2), "Should successfully add node 2");
    TEST_ASSERT(!g.addNode(n1), "Duplicate node addition should fail");
    TEST_ASSERT(g.nodeCount() == 2, "Node count should be 2");

    Edge e1("user_service", "order_service", RelationshipType::SERVICE_DEPENDS_ON_SERVICE);
    TEST_ASSERT(g.addEdge(e1), "Should successfully add edge");
    TEST_ASSERT(!g.addEdge(e1), "Duplicate edge addition should fail");
    TEST_ASSERT(g.edgeCount() == 1, "Edge count should be 1");

    TEST_ASSERT(g.hasEdge("user_service", "order_service"), "Should detect existing edge");
    TEST_ASSERT(!g.hasEdge("order_service", "user_service"), "Should not detect reverse non-existent edge");

    return true;
}

bool testNodeDeletionCascadesEdges() {
    Graph g;
    g.addNode(Node("A", NodeType::SERVICE, "Service A"));
    g.addNode(Node("B", NodeType::SERVICE, "Service B"));
    g.addNode(Node("C", NodeType::SERVICE, "Service C"));

    g.addEdge(Edge("A", "B", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(Edge("B", "C", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));

    TEST_ASSERT(g.edgeCount() == 2, "Initial edge count should be 2");
    TEST_ASSERT(g.removeNode("B"), "Removing node B should succeed");
    TEST_ASSERT(g.nodeCount() == 2, "Node count should be 2 after deletion");
    TEST_ASSERT(g.edgeCount() == 0, "All incident edges to B should be deleted");
    TEST_ASSERT(g.getOutgoingEdges("A").empty(), "Outgoing edges from A should be empty");
    TEST_ASSERT(g.getIncomingEdges("C").empty(), "Incoming edges to C should be empty");

    return true;
}

bool testBfsAndDfsTraversal() {
    Graph g;
    // Tree:
    //      1
    //    /   \
    //   2     3
    //  / \
    // 4   5
    g.addNode(Node("1", NodeType::SERVICE, "Node 1"));
    g.addNode(Node("2", NodeType::SERVICE, "Node 2"));
    g.addNode(Node("3", NodeType::SERVICE, "Node 3"));
    g.addNode(Node("4", NodeType::SERVICE, "Node 4"));
    g.addNode(Node("5", NodeType::SERVICE, "Node 5"));

    g.addEdge(Edge("1", "2", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(Edge("1", "3", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(Edge("2", "4", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    g.addEdge(Edge("2", "5", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));

    auto bfsOrder = GraphAlgorithms::bfs(g, "1");
    TEST_ASSERT(bfsOrder.size() == 5, "BFS should visit all 5 nodes");
    TEST_ASSERT(bfsOrder[0] == "1", "BFS root must be 1");
    TEST_ASSERT(bfsOrder[1] == "2" || bfsOrder[1] == "3", "BFS level 1");

    auto dfsOrder = GraphAlgorithms::dfs(g, "1");
    TEST_ASSERT(dfsOrder.size() == 5, "DFS should visit all 5 nodes");
    TEST_ASSERT(dfsOrder[0] == "1", "DFS root must be 1");

    return true;
}

bool testCycleDetectionAndTopologicalSort() {
    Graph dag;
    dag.addNode(Node("A", NodeType::SERVICE, "A"));
    dag.addNode(Node("B", NodeType::SERVICE, "B"));
    dag.addNode(Node("C", NodeType::SERVICE, "C"));
    dag.addNode(Node("D", NodeType::SERVICE, "D"));

    dag.addEdge(Edge("A", "B", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    dag.addEdge(Edge("A", "C", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    dag.addEdge(Edge("B", "D", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    dag.addEdge(Edge("C", "D", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));

    TEST_ASSERT(!GraphAlgorithms::hasCycle(dag), "DAG must have no cycle");

    auto topoOpt = GraphAlgorithms::topologicalSort(dag);
    TEST_ASSERT(topoOpt.has_value(), "Topological sort should succeed for DAG");
    auto topo = *topoOpt;
    TEST_ASSERT(topo.size() == 4, "Topological sort size must match node count");

    auto posA = std::find(topo.begin(), topo.end(), "A");
    auto posB = std::find(topo.begin(), topo.end(), "B");
    auto posC = std::find(topo.begin(), topo.end(), "C");
    auto posD = std::find(topo.begin(), topo.end(), "D");
    TEST_ASSERT(posA < posB, "A must precede B");
    TEST_ASSERT(posA < posC, "A must precede C");
    TEST_ASSERT(posB < posD, "B must precede D");
    TEST_ASSERT(posC < posD, "C must precede D");

    // Introduce cycle: D -> A
    Graph cyclic = dag;
    cyclic.addEdge(Edge("D", "A", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    TEST_ASSERT(GraphAlgorithms::hasCycle(cyclic), "Cyclic graph must be detected as having a cycle");

    auto cyclicTopo = GraphAlgorithms::topologicalSort(cyclic);
    TEST_ASSERT(!cyclicTopo.has_value(), "Topological sort must return nullopt for cyclic graph");

    auto cycles = GraphAlgorithms::findCycles(cyclic);
    TEST_ASSERT(!cycles.empty(), "findCycles must find the cycle");

    return true;
}

bool testImpactReachabilityAndShortestPath() {
    Graph g;
    // Microservices:
    // Client -> Gateway -> Auth Service -> Database
    // Gateway -> Order Service -> Payment Service
    g.addNode(Node("client", NodeType::CLIENT, "Mobile Client"));
    g.addNode(Node("gateway", NodeType::SERVICE, "API Gateway"));
    g.addNode(Node("auth", NodeType::SERVICE, "Auth Service"));
    g.addNode(Node("order", NodeType::SERVICE, "Order Service"));
    g.addNode(Node("payment", NodeType::SERVICE, "Payment Service"));

    g.addEdge(Edge("client", "gateway", RelationshipType::CLIENT_CALLS_OPERATION));
    g.addEdge(Edge("gateway", "auth", RelationshipType::OPERATION_CALLS_OPERATION));
    g.addEdge(Edge("gateway", "order", RelationshipType::OPERATION_CALLS_OPERATION));
    g.addEdge(Edge("order", "payment", RelationshipType::OPERATION_CALLS_OPERATION));

    // Reachability
    TEST_ASSERT(GraphAlgorithms::isReachable(g, "client", "payment"), "Client should reach payment");
    TEST_ASSERT(!GraphAlgorithms::isReachable(g, "payment", "client"), "Payment should not reach client");

    // Shortest Path
    auto pathOpt = GraphAlgorithms::shortestPath(g, "client", "payment");
    TEST_ASSERT(pathOpt.has_value(), "Path from client to payment should exist");
    std::vector<std::string> expected = {"client", "gateway", "order", "payment"};
    TEST_ASSERT(*pathOpt == expected, "Shortest path should match client -> gateway -> order -> payment");

    // Reverse Reachability (Blast Radius / Impact Analysis):
    // If 'payment' changes, who depends on it? Order, Gateway, Client!
    auto impacted = GraphAlgorithms::findReverseReachableNodes(g, "payment");
    TEST_ASSERT(impacted.size() == 4, "Impacted nodes count should be 4 (payment, order, gateway, client)");
    TEST_ASSERT(std::find(impacted.begin(), impacted.end(), "order") != impacted.end(), "Order must be impacted");
    TEST_ASSERT(std::find(impacted.begin(), impacted.end(), "gateway") != impacted.end(), "Gateway must be impacted");
    TEST_ASSERT(std::find(impacted.begin(), impacted.end(), "client") != impacted.end(), "Client must be impacted");
    TEST_ASSERT(std::find(impacted.begin(), impacted.end(), "auth") == impacted.end(), "Auth should NOT be impacted");

    return true;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "TraceWeaver Graph Engine Test Suite (Phase 1)\n";
    std::cout << "========================================\n";

    RUN_TEST(testGraphCreationAndBasicOps);
    RUN_TEST(testNodeDeletionCascadesEdges);
    RUN_TEST(testBfsAndDfsTraversal);
    RUN_TEST(testCycleDetectionAndTopologicalSort);
    RUN_TEST(testImpactReachabilityAndShortestPath);

    std::cout << "========================================\n";
    std::cout << "Summary: " << g_testsPassed << " passed, " << g_testsFailed << " failed.\n";
    std::cout << "========================================\n";

    return (g_testsFailed == 0) ? 0 : 1;
}
