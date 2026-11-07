# TraceWeaver Architecture Documentation

## 1. System Mission & Core Value Proposition
TraceWeaver is an enterprise-grade API Change Impact Analysis and Regression Replay platform. Its primary mission is to enable developers and platform engineering teams to **"Understand what could break before an API change reaches production."**

The platform maintains a strict boundary between:
1. **Potential Impact**: Derived via multi-tier directed dependency graph traversal.
2. **Contract Breaking Changes**: Detected deterministically via OpenAPI structural diffing.
3. **Confirmed Regressions**: Verified by executing sanitized replay traffic against the target runtime.

---

## 2. High-Level System Architecture

```
React / TypeScript Dashboard (React Flow)
                 │
                 ▼
        REST API Gateway (/api/v1)
                 │
     ┌───────────┴───────────┐
     ▼                       ▼
PostgreSQL (Storage)    Redis (Job Queue)
     │                       │
     └───────────┬───────────┘
                 ▼
     C++20 Core Analysis Engine
     ├── Specification Parser & Normalizer
     ├── Contract Diff Engine
     ├── Multi-Tier Dependency Graph
     ├── Impact Analysis & Blast Radius Traversal
     ├── Explainable Risk Scorer (0-100)
     ├── Selective Regression Test Engine
     └── Request Replay & Response Comparator
```

---

## 3. Logical Engine Modules

### 3.1 Specification Parser & Schema Normalizer
- Supports OpenAPI 3.0 and 3.1 definitions in JSON and YAML.
- Normalizes operations, parameters, request bodies, and response definitions into an internal `ApiSpecification` data model.
- Recursively resolves `$ref` pointers with memoized cycle detection to prevent infinite loops on recursive schemas.

### 3.2 Contract Diff Engine
- Performs deterministic structural comparison without AI hallucination.
- Emits structured `ApiChange` events categorized by:
  - `BREAKING` (e.g. response field removed, required request field added, type mutation).
  - `POTENTIALLY_BREAKING` (e.g. enum value removed, query parameter dropped).
  - `NON_BREAKING` (e.g. optional response property introduced, new endpoint added).

### 3.3 Dependency Graph Engine
- Implemented as an in-memory directed graph using dual adjacency lists (`outgoingEdges_` and `incomingEdges_`).
- Provides $O(1)$ predecessor lookups for upstream impact traversal.
- Algorithms implemented: BFS, DFS, 3-color Cycle Detection, Kahn's Topological Sorting, Shortest Path, and Reverse Reachability.

### 3.4 Impact Analysis & Blast Radius Engine
- Propagates breaking changes upstream through the dependency graph.
- Distinguishes between direct callers (`DIRECT`, depth 1) and transitive callers (`INDIRECT`, depth 2+).

### 3.5 Explainable Risk Scoring Engine
- Computes normalized score between 0 and 100 based on:
  $$\text{Risk Score} = \text{Severity} (30) + \text{Breadth} (25) + \text{Depth} (15) + \text{Criticality} (15) + \text{Replay Failures} (15)$$
- Classifies risk into `LOW` (0-29), `MEDIUM` (30-59), `HIGH` (60-84), and `CRITICAL` (85-100).

### 3.6 Selective Test Selector
- Maps affected operations to associated test cases.
- Provides **Safe Mode** (all impacted tests) and **Optimized Mode** (ranked, deduplicated minimal test set).

### 3.7 Replay & Response Comparator
- Replays sanitized historical production traffic against staging targets.
- Redacts authorization tokens, cookies, passwords, and sensitive keys.
- Categorizes regressions into `STRUCTURAL_REGRESSION`, `STATUS_REGRESSION`, `VALUE_REGRESSION`, and `PERFORMANCE_REGRESSION`.
