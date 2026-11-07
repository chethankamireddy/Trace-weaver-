# TraceWeaver Algorithmic Foundations

This document details the core data structures and algorithms powering the TraceWeaver C++20 engine.

---

## 1. Dual Adjacency List Directed Graph

### Complexity:
- **Node Insertion**: $O(1)$ amortized
- **Edge Insertion**: $O(\text{deg}(u))$ check for duplicates, $O(1)$ average
- **Predecessor Lookup**: $O(1)$ time to access the vector of incoming edges
- **Successor Lookup**: $O(1)$ time to access the vector of outgoing edges

### Storage Representation:
```cpp
std::unordered_map<std::string, Node> nodes_;
std::unordered_map<std::string, std::vector<Edge>> outgoingEdges_; // u -> v
std::unordered_map<std::string, std::vector<Edge>> incomingEdges_; // v <- u
```

**Interview Rationale**:
In standard graphs with only outgoing edges, finding upstream dependencies requires an $O(|V| + |E|)$ scan. Maintaining `incomingEdges_` enables direct $O(|E_{\text{upstream}}|)$ traversal for impact analysis.

---

## 2. Cycle Detection (3-Color DFS)

### States:
- `WHITE`: Node has not yet been discovered.
- `GRAY`: Node is currently on the active recursion stack (ancestor).
- `BLACK`: Node and all its descendants have been fully explored.

### Invariant:
A directed graph contains a cycle if and only if a DFS traversal encounters an edge from the current node $u$ to a `GRAY` node $v$ (a back-edge).

$$\text{Time Complexity}: O(|V| + |E|) \quad | \quad \text{Space Complexity}: O(|V|)$$

---

## 3. Topological Sorting (Kahn's Algorithm)

### Mechanism:
1. Compute the in-degree of all nodes: $O(|V| + |E|)$.
2. Enqueue all nodes with in-degree 0 into a FIFO queue.
3. While the queue is non-empty:
   - Pop node $u$, append to `sortedOrder`.
   - For each neighbor $v$ of $u$, decrement $\text{in-degree}[v]$.
   - If $\text{in-degree}[v] == 0$, enqueue $v$.
4. **Cycle Verification**: If $|\text{sortedOrder}| \neq |V|$, a directed cycle exists; return `std::nullopt`.

---

## 4. Upstream Impact Traversal (Reverse BFS)

```cpp
// Traversal queue stores {nodeId, depth}
std::queue<std::pair<std::string, size_t>> q;
q.push({modifiedOperationId, 0});

while (!q.empty()) {
    auto [currentId, depth] = q.front();
    q.pop();

    for (const auto& edge : graph.getIncomingEdges(currentId)) {
        const std::string& upstream = edge.getSourceId();
        if (!visited.contains(upstream)) {
            visited.insert(upstream);
            parentMap[upstream] = currentId;
            q.push({upstream, depth + 1});
        }
    }
}
```

- Components reached at `depth == 1` are classified as **`DIRECT`** impact.
- Components reached at `depth >= 2` are classified as **`INDIRECT`** impact.

---

## 5. Explainable Risk Scoring Formulation

$$\text{Score} = \min(30, S) + \min(25, B) + \min(15, D) + C + \min(15, R)$$

Where:
- $S$ (**Severity**): $15$ pts per Critical Breaking, $10$ pts per High Breaking, $4$ pts per Potentially Breaking.
- $B$ (**Breadth**): $6 \times \text{Services} + 5 \times \text{Clients} + 2 \times \text{Operations}$.
- $D$ (**Depth**): $0$ (depth 0), $5$ (depth 1), $10$ (depth 2), $15$ (depth $\ge 3$).
- $C$ (**Criticality**): Low (3), Medium (7), High (11), Critical (15).
- $R$ (**Regression Ratio**): $15 \times \frac{\text{Failed Replay Requests}}{\text{Total Replay Requests}}$.
