#include <gtest/gtest.h>
#include "graph/Graph.hpp"
#include "graph/Algorithms.hpp"

using namespace traceweaver::graph;

class GraphTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Build a microservice dependency graph:
        // User Service (US) -> Order Service (OS)
        // OS -> Inventory Service (IS)
        // OS -> Payment Service (PS)
        // OS -> Notification Service (NS)
        graph.addNode(Node("US", NodeType::SERVICE, "User Service"));
        graph.addNode(Node("OS", NodeType::SERVICE, "Order Service"));
        graph.addNode(Node("IS", NodeType::SERVICE, "Inventory Service"));
        graph.addNode(Node("PS", NodeType::SERVICE, "Payment Service"));
        graph.addNode(Node("NS", NodeType::SERVICE, "Notification Service"));

        graph.addEdge(Edge("US", "OS", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
        graph.addEdge(Edge("OS", "IS", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
        graph.addEdge(Edge("OS", "PS", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
        graph.addEdge(Edge("OS", "NS", RelationshipType::SERVICE_DEPENDS_ON_SERVICE));
    }
};

TEST_F(GraphTest, NodeAndEdgeCount) {
    EXPECT_EQ(graph.nodeCount(), 5);
    EXPECT_EQ(graph.edgeCount(), 4);
    EXPECT_TRUE(graph.hasNode("US"));
    EXPECT_FALSE(graph.hasNode("NON_EXISTENT"));
}

TEST_F(GraphTest, NodeMetadataAndLookup) {
    auto nodeOpt = graph.getNode("US");
    ASSERT_TRUE(nodeOpt.has_value());
    EXPECT_EQ(nodeOpt->get().getName(), "User Service");
    EXPECT_EQ(nodeOpt->get().getType(), NodeType::SERVICE);
    EXPECT_EQ(nodeOpt->get().getTypeName(), "SERVICE");
}

TEST_F(GraphTest, SuccessorsAndPredecessors) {
    auto successors = graph.getSuccessors("OS");
    EXPECT_EQ(successors.size(), 3);

    auto predecessors = graph.getPredecessors("OS");
    ASSERT_EQ(predecessors.size(), 1);
    EXPECT_EQ(predecessors[0], "US");
}

TEST_F(GraphTest, RemoveNodeCascadesEdges) {
    EXPECT_TRUE(graph.removeNode("OS"));
    EXPECT_FALSE(graph.hasNode("OS"));
    EXPECT_EQ(graph.nodeCount(), 4);
    // Removing OS removes: (US->OS), (OS->IS), (OS->PS), (OS->NS) -> 4 edges removed
    EXPECT_EQ(graph.edgeCount(), 0);
    EXPECT_TRUE(graph.getOutgoingEdges("US").empty());
    EXPECT_TRUE(graph.getIncomingEdges("IS").empty());
}

TEST_F(GraphTest, Reachability) {
    EXPECT_TRUE(GraphAlgorithms::isReachable(graph, "US", "IS"));
    EXPECT_TRUE(GraphAlgorithms::isReachable(graph, "US", "NS"));
    EXPECT_FALSE(GraphAlgorithms::isReachable(graph, "IS", "US"));
    EXPECT_FALSE(GraphAlgorithms::isReachable(graph, "PS", "NS"));
}

TEST_F(GraphTest, ShortestPath) {
    auto path = GraphAlgorithms::shortestPath(graph, "US", "PS");
    ASSERT_TRUE(path.has_value());
    std::vector<std::string> expected = {"US", "OS", "PS"};
    EXPECT_EQ(*path, expected);

    auto noPath = GraphAlgorithms::shortestPath(graph, "IS", "US");
    EXPECT_FALSE(noPath.has_value());
}

TEST_F(GraphTest, TopologicalSortOnDAG) {
    EXPECT_FALSE(GraphAlgorithms::hasCycle(graph));
    auto sorted = GraphAlgorithms::topologicalSort(graph);
    ASSERT_TRUE(sorted.has_value());
    EXPECT_EQ(sorted->size(), 5);

    // In topological sort, US must appear before OS, and OS before IS, PS, NS
    auto posUS = std::find(sorted->begin(), sorted->end(), "US");
    auto posOS = std::find(sorted->begin(), sorted->end(), "OS");
    auto posIS = std::find(sorted->begin(), sorted->end(), "IS");
    EXPECT_LT(posUS, posOS);
    EXPECT_LT(posOS, posIS);
}

TEST_F(GraphTest, CycleDetection) {
    // Add cycle: NS -> US
    EXPECT_TRUE(graph.addEdge(Edge("NS", "US", RelationshipType::SERVICE_DEPENDS_ON_SERVICE)));
    EXPECT_TRUE(GraphAlgorithms::hasCycle(graph));

    auto cycles = GraphAlgorithms::findCycles(graph);
    EXPECT_FALSE(cycles.empty());

    auto sorted = GraphAlgorithms::topologicalSort(graph);
    EXPECT_FALSE(sorted.has_value()); // Kahn's algorithm fails on cycles
}

TEST_F(GraphTest, ReverseReachabilityForImpactAnalysis) {
    // If IS changes, who depends on it?
    // In our graph: US -> OS -> IS
    // Reverse reachable from IS should be: IS, OS, US
    auto impacted = GraphAlgorithms::findReverseReachableNodes(graph, "IS");
    EXPECT_EQ(impacted.size(), 3);
    EXPECT_TRUE(std::find(impacted.begin(), impacted.end(), "IS") != impacted.end());
    EXPECT_TRUE(std::find(impacted.begin(), impacted.end(), "OS") != impacted.end());
    EXPECT_TRUE(std::find(impacted.begin(), impacted.end(), "US") != impacted.end());
}
