# TraceWeaver REST API Design Specification

All endpoints are versioned under `/api/v1/` and exchange JSON payloads.

---

## 1. Workspaces & Projects
- `GET /api/v1/workspaces`: List user workspaces.
- `POST /api/v1/workspaces`: Create workspace.
- `GET /api/v1/projects`: List projects within workspace.
- `POST /api/v1/projects`: Create project.

---

## 2. Services & Specifications
- `GET /api/v1/services`: List microservices in catalog.
- `POST /api/v1/services`: Register new microservice.
- `GET /api/v1/services/:id/specifications`: List OpenAPI contract versions.
- `POST /api/v1/services/:id/specifications`: Upload new OpenAPI 3.0/3.1 contract.

---

## 3. Dependency Graph Endpoints
- `GET /api/v1/projects/:id/dependency-graph`: Fetch complete directed graph (nodes and edges).
- `GET /api/v1/projects/:id/dependency-graph/nodes`: List nodes with metadata.
- `GET /api/v1/projects/:id/dependency-graph/edges`: List relationship edges.

---

## 4. Change Impact Analysis Endpoints
- `POST /api/v1/analysis-runs`: Initiate change impact analysis comparing two versions.
- `GET /api/v1/analysis-runs/:id`: Fetch analysis summary and execution status.
- `GET /api/v1/analysis-runs/:id/changes`: Fetch detected breaking and non-breaking contract changes.
- `GET /api/v1/analysis-runs/:id/impact`: Fetch impacted components in blast radius.
- `GET /api/v1/analysis-runs/:id/risk`: Fetch explainable 0-100 risk score and breakdown.
- `GET /api/v1/analysis-runs/:id/tests`: Fetch selective regression test recommendations.

---

## 5. Request Replay Endpoints
- `POST /api/v1/replay-runs`: Trigger request replay against a target URL.
- `GET /api/v1/replay-runs/:id`: Fetch replay run progress and summary statistics.
- `GET /api/v1/replay-runs/:id/results`: Fetch request-by-request diff comparison results.

---

## 6. GitHub CI/CD Webhook
- `POST /api/v1/integrations/github/webhook`: Webhook receiving pull request events, executing automated impact analysis, and publishing markdown comments.
