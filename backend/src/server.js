const http = require('http');
const url = require('url');
const fs = require('fs');
const path = require('path');

const PORT = process.env.PORT || 4000;

// In-Memory Data Store seeded with Demo Architecture
let workspaces = [
  {
    id: "ws_default",
    name: "Enterprise Retail E-Commerce",
    slug: "retail-ecommerce",
    description: "Production microservices platform for global digital retail",
    createdAt: new Date().toISOString()
  }
];

let projects = [
  {
    id: "proj_ecommerce",
    workspaceId: "ws_default",
    name: "Core Commerce Platform",
    slug: "core-commerce",
    description: "Multi-service distributed architecture handling checkout, payments, and users",
    createdAt: new Date().toISOString()
  }
];

// Load demo microservices
const demoServicesPath = path.resolve(__dirname, '../../demo/services/services.json');
let services = fs.existsSync(demoServicesPath) 
  ? JSON.parse(fs.readFileSync(demoServicesPath, 'utf8'))
  : [];

// Load specifications
const specV1Path = path.resolve(__dirname, '../../demo/specifications/user-service-v1.json');
const specV2Path = path.resolve(__dirname, '../../demo/specifications/user-service-v2.json');
const specV1 = fs.existsSync(specV1Path) ? JSON.parse(fs.readFileSync(specV1Path, 'utf8')) : {};
const specV2 = fs.existsSync(specV2Path) ? JSON.parse(fs.readFileSync(specV2Path, 'utf8')) : {};

let specifications = [
  {
    id: "spec_user_service",
    serviceId: "srv_user",
    title: "User Service OpenAPI Spec",
    currentVersion: "2.0.0",
    specType: "OPENAPI_3",
    versions: [
      {
        versionTag: "1.0.0",
        rawContent: JSON.stringify(specV1, null, 2),
        createdAt: "2026-10-15T08:00:00Z"
      },
      {
        versionTag: "2.0.0",
        rawContent: JSON.stringify(specV2, null, 2),
        createdAt: "2026-11-01T12:00:00Z"
      }
    ]
  }
];

// Graph Nodes & Edges
let dependencyNodes = [
  { id: "srv_user", type: "SERVICE", name: "User Service", description: "Identity & Accounts", metadata: { criticality: "HIGH", env: "production" } },
  { id: "srv_order", type: "SERVICE", name: "Order Service", description: "Order Lifecycle & Cart", metadata: { criticality: "CRITICAL", env: "production" } },
  { id: "srv_product", type: "SERVICE", name: "Product Service", description: "Catalog & Pricing", metadata: { criticality: "MEDIUM", env: "production" } },
  { id: "srv_inventory", type: "SERVICE", name: "Inventory Service", description: "Warehouse Stock", metadata: { criticality: "HIGH", env: "production" } },
  { id: "srv_payment", type: "SERVICE", name: "Payment Service", description: "Billing & Cards", metadata: { criticality: "CRITICAL", env: "production" } },
  { id: "srv_notification", type: "SERVICE", name: "Notification Service", description: "Email & Push SMS", metadata: { criticality: "MEDIUM", env: "production" } },
  { id: "srv_recommendation", type: "SERVICE", name: "Recommendation Service", description: "ML Feeds", metadata: { criticality: "LOW", env: "staging" } },
  { id: "client_mobile", type: "CLIENT", name: "Mobile App (iOS/Android)", description: "Native Mobile Consumer", metadata: { tier: "client" } },
  { id: "client_web", type: "CLIENT", name: "Web Storefront (React)", description: "Public E-Commerce Web", metadata: { tier: "client" } },
  { id: "op_getUserById", type: "API_OPERATION", name: "GET /api/v1/users/{id}", description: "User Retrieval", metadata: { method: "GET" } },
  { id: "op_createOrder", type: "API_OPERATION", name: "POST /api/v1/orders", description: "Checkout Order", metadata: { method: "POST" } }
];

let dependencyEdges = [
  { source: "client_mobile", target: "srv_user", relationship: "CLIENT_CALLS_OPERATION", weight: 1.0 },
  { source: "client_web", target: "srv_order", relationship: "CLIENT_CALLS_OPERATION", weight: 1.0 },
  { source: "srv_order", target: "srv_user", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_order", target: "srv_inventory", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_order", target: "srv_payment", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_order", target: "srv_notification", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_recommendation", target: "srv_user", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_recommendation", target: "srv_product", relationship: "SERVICE_DEPENDS_ON_SERVICE", weight: 1.0 },
  { source: "srv_user", target: "op_getUserById", relationship: "OPERATION_USES_SCHEMA", weight: 1.0 },
  { source: "srv_order", target: "op_createOrder", relationship: "OPERATION_USES_SCHEMA", weight: 1.0 }
];

// Test Cases
const testsPath = path.resolve(__dirname, '../../demo/test-cases/tests.json');
let testCases = fs.existsSync(testsPath) ? JSON.parse(fs.readFileSync(testsPath, 'utf8')) : [];

// Historical Recordings
const recsPath = path.resolve(__dirname, '../../demo/requests/historical_recordings.json');
let requestRecordings = fs.existsSync(recsPath) ? JSON.parse(fs.readFileSync(recsPath, 'utf8')) : [];

// Analysis Runs
let analysisRuns = [
  {
    id: "run_user_v1_to_v2",
    projectId: "proj_ecommerce",
    serviceId: "srv_user",
    serviceName: "User Service",
    oldVersion: "1.0.0",
    newVersion: "2.0.0",
    status: "COMPLETED",
    riskScore: 84.5,
    riskClassification: "HIGH",
    createdAt: "2026-11-07T10:00:00Z",
    completedAt: "2026-11-07T10:00:03Z",
    summary: {
      breakingChanges: 3,
      potentiallyBreaking: 1,
      nonBreaking: 2,
      impactedServices: 3,
      impactedClients: 1,
      selectedTests: 4,
      totalTests: 6
    }
  }
];

let detectedChanges = [
  {
    id: "chg_field_rem_name",
    analysisRunId: "run_user_v1_to_v2",
    type: "RESPONSE_FIELD_REMOVED",
    category: "RESPONSE_FIELD",
    severity: "CRITICAL",
    compatibility: "BREAKING",
    oldValue: "name (string)",
    newValue: "[REMOVED]",
    jsonPointer: "/paths/~1api~1v1~1users~1{id}/responses/200/content/application~1json/schema/properties/name",
    operationId: "getUserById",
    description: "Response field 'name' was completely removed from UserProfile schema",
    reason: "Downstream consumers (Order Service, Notification Service, Mobile App) expecting 'name' will fail with null pointer or deserialization crashes."
  },
  {
    id: "chg_field_type_tier",
    analysisRunId: "run_user_v1_to_v2",
    type: "RESPONSE_FIELD_TYPE_CHANGED",
    category: "RESPONSE_FIELD",
    severity: "HIGH",
    compatibility: "BREAKING",
    oldValue: "tier (string)",
    newValue: "tier (integer)",
    jsonPointer: "/components/schemas/UserProfile/properties/tier",
    operationId: "getUserById",
    description: "Field 'tier' changed type from string to integer",
    reason: "Strict JSON type deserialization in downstream microservices will fail."
  },
  {
    id: "chg_enum_rem_suspended",
    analysisRunId: "run_user_v1_to_v2",
    type: "ENUM_VALUES_REMOVED",
    category: "RESPONSE_FIELD",
    severity: "MEDIUM",
    compatibility: "POTENTIALLY_BREAKING",
    oldValue: "enum ['ACTIVE', 'SUSPENDED', 'PENDING']",
    newValue: "enum ['ACTIVE', 'PENDING']",
    jsonPointer: "/components/schemas/UserProfile/properties/status",
    operationId: "getUserById",
    description: "Enum value 'SUSPENDED' was removed from user status",
    reason: "Consumers expecting 'SUSPENDED' may experience switch-case fallthrough or unhandled states."
  },
  {
    id: "chg_req_field_age",
    analysisRunId: "run_user_v1_to_v2",
    type: "REQUIRED_REQUEST_FIELD_ADDED",
    category: "REQUEST_BODY",
    severity: "HIGH",
    compatibility: "BREAKING",
    oldValue: "[NONE]",
    newValue: "ageVerification (boolean, required)",
    jsonPointer: "/components/schemas/CreateUserRequest/properties/ageVerification",
    operationId: "createUser",
    description: "New required boolean field 'ageVerification' added to user registration request",
    reason: "Existing callers without this parameter will fail HTTP 400 validation."
  },
  {
    id: "chg_field_add_avatar",
    analysisRunId: "run_user_v1_to_v2",
    type: "OPTIONAL_RESPONSE_FIELD_ADDED",
    category: "RESPONSE_FIELD",
    severity: "LOW",
    compatibility: "NON_BREAKING",
    oldValue: "[NONE]",
    newValue: "avatarUrl (string)",
    jsonPointer: "/components/schemas/UserProfile/properties/avatarUrl",
    operationId: "getUserById",
    description: "Optional response field 'avatarUrl' introduced",
    reason: "Tolerant consumers can safely ignore unknown additional response properties."
  }
];

let impactedComponents = [
  {
    id: "imp_1",
    analysisRunId: "run_user_v1_to_v2",
    nodeId: "srv_order",
    componentName: "Order Service",
    componentType: "SERVICE",
    impactLevel: "DIRECT",
    depth: 1,
    dependencyPath: ["srv_order", "srv_user"],
    reason: "Direct dependency: Order Service reads 'name' field during checkout verification"
  },
  {
    id: "imp_2",
    analysisRunId: "run_user_v1_to_v2",
    nodeId: "client_mobile",
    componentName: "Mobile App (iOS/Android)",
    componentType: "CLIENT",
    impactLevel: "DIRECT",
    depth: 1,
    dependencyPath: ["client_mobile", "srv_user"],
    reason: "Direct client consumer: User profile screen displays 'name' directly from response"
  },
  {
    id: "imp_3",
    analysisRunId: "run_user_v1_to_v2",
    nodeId: "srv_notification",
    componentName: "Notification Service",
    componentType: "SERVICE",
    impactLevel: "INDIRECT",
    depth: 2,
    dependencyPath: ["srv_notification", "srv_order", "srv_user"],
    reason: "Indirect dependency: Notification service templates user name into dispatch emails"
  },
  {
    id: "imp_4",
    analysisRunId: "run_user_v1_to_v2",
    nodeId: "srv_recommendation",
    componentName: "Recommendation Service",
    componentType: "SERVICE",
    impactLevel: "DIRECT",
    depth: 1,
    dependencyPath: ["srv_recommendation", "srv_user"],
    reason: "Direct dependency: Recommendation model parses 'tier' and enum status"
  }
];

let replayRuns = [
  {
    id: "rep_user_staging",
    analysisRunId: "run_user_v1_to_v2",
    serviceId: "srv_user",
    serviceName: "User Service",
    targetEnvironment: "http://staging-user-service.internal:8080",
    status: "COMPLETED",
    totalRequests: 2,
    passedRequests: 0,
    failedRequests: 2,
    createdAt: "2026-11-07T10:05:00Z",
    results: [
      {
        recordingId: "rec_usr_001",
        primaryRegression: "STRUCTURAL_REGRESSION",
        isFailure: true,
        expectedStatus: 200,
        actualStatus: 200,
        historicalLatencyMs: 42.0,
        replayLatencyMs: 45.0,
        summaryDiff: "Missing required field 'name' in response; found new field 'fullName': 'Alex Chen'",
        fieldDifferences: [
          { jsonPointer: "/name", differenceType: "MISSING_FIELD", expectedValue: "Alex Chen", actualValue: "undefined" },
          { jsonPointer: "/fullName", differenceType: "EXTRA_FIELD", expectedValue: "undefined", actualValue: "Alex Chen" }
        ]
      },
      {
        recordingId: "rec_usr_002",
        primaryRegression: "STRUCTURAL_REGRESSION",
        isFailure: true,
        expectedStatus: 200,
        actualStatus: 200,
        historicalLatencyMs: 38.0,
        replayLatencyMs: 41.0,
        summaryDiff: "Missing required field 'name' in response; found new field 'fullName': 'Sarah Connor'",
        fieldDifferences: [
          { jsonPointer: "/name", differenceType: "MISSING_FIELD", expectedValue: "Sarah Connor", actualValue: "undefined" }
        ]
      }
    ]
  }
];

// Helper to send JSON responses with CORS headers
function sendJson(res, statusCode, data) {
  res.writeHead(statusCode, {
    'Content-Type': 'application/json',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, PUT, PATCH, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type, Authorization, X-Requested-With'
  });
  res.end(JSON.stringify(data));
}

// Request dispatcher
const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);
  let pathname = parsedUrl.pathname || '/';
  const method = req.method;

  // Handle CORS preflight
  if (method === 'OPTIONS') {
    res.writeHead(204, {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, PATCH, DELETE, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type, Authorization, X-Requested-With'
    });
    return res.end();
  }

  // SERVE FRONTEND DIRECTLY ON ROOT PATHS
  if ((pathname === '/' || pathname === '/index.html' || pathname === '') && method === 'GET') {
    const indexPath = path.resolve(__dirname, '../../frontend/index.html');
    if (fs.existsSync(indexPath)) {
      const htmlContent = fs.readFileSync(indexPath, 'utf8');
      res.writeHead(200, {
        'Content-Type': 'text/html; charset=utf-8',
        'Access-Control-Allow-Origin': '*'
      });
      return res.end(htmlContent);
    }
  }

  // Parse JSON body for POST/PATCH
  let bodyBuffer = '';
  req.on('data', chunk => { bodyBuffer += chunk; });
  req.on('end', () => {
    let body = {};
    if (bodyBuffer) {
      try { body = JSON.parse(bodyBuffer); } catch (e) {}
    }

    // Router
    if (pathname === '/api/v1/health' && method === 'GET') {
      return sendJson(res, 200, { status: 'UP', engine: 'TraceWeaver C++20 Core Bridge', timestamp: new Date().toISOString() });
    }

    if (pathname === '/api/v1/workspaces' && method === 'GET') {
      return sendJson(res, 200, workspaces);
    }
    if (pathname === '/api/v1/projects' && method === 'GET') {
      return sendJson(res, 200, projects);
    }
    if (pathname === '/api/v1/services' && method === 'GET') {
      return sendJson(res, 200, services);
    }
    if (pathname.match(/^\/api\/v1\/projects\/[^\/]+\/services$/) && method === 'GET') {
      return sendJson(res, 200, services);
    }

    // Dependency Graph
    if (pathname.match(/^\/api\/v1\/projects\/[^\/]+\/dependency-graph$/) && method === 'GET') {
      return sendJson(res, 200, {
        nodes: dependencyNodes,
        edges: dependencyEdges,
        totalNodes: dependencyNodes.length,
        totalEdges: dependencyEdges.length
      });
    }

    // Analysis Runs
    if (pathname === '/api/v1/analysis-runs' && method === 'GET') {
      return sendJson(res, 200, analysisRuns);
    }
    if (pathname === '/api/v1/analysis-runs' && method === 'POST') {
      const srv = services.find(s => s.id === body.serviceId) || { name: "User Service" };
      const newRun = {
        id: `run_${Date.now()}`,
        projectId: "proj_ecommerce",
        serviceId: body.serviceId || "srv_user",
        serviceName: srv.name,
        oldVersion: body.oldVersion || "1.0.0",
        newVersion: body.newVersion || "2.0.0",
        status: "COMPLETED",
        riskScore: 84.5,
        riskClassification: "HIGH",
        createdAt: new Date().toISOString(),
        completedAt: new Date().toISOString(),
        summary: {
          breakingChanges: 3,
          potentiallyBreaking: 1,
          nonBreaking: 2,
          impactedServices: 3,
          impactedClients: 1,
          selectedTests: 4,
          totalTests: 6
        }
      };
      analysisRuns.unshift(newRun);
      return sendJson(res, 201, newRun);
    }

    const runMatch = pathname.match(/^\/api\/v1\/analysis-runs\/([^\/]+)$/);
    if (runMatch && method === 'GET') {
      const run = analysisRuns.find(r => r.id === runMatch[1]);
      return run ? sendJson(res, 200, run) : sendJson(res, 404, { error: 'Not found' });
    }

    const changesMatch = pathname.match(/^\/api\/v1\/analysis-runs\/([^\/]+)\/changes$/);
    if (changesMatch && method === 'GET') {
      return sendJson(res, 200, detectedChanges);
    }

    const impactMatch = pathname.match(/^\/api\/v1\/analysis-runs\/([^\/]+)\/impact$/);
    if (impactMatch && method === 'GET') {
      return sendJson(res, 200, impactedComponents);
    }

    const riskMatch = pathname.match(/^\/api\/v1\/analysis-runs\/([^\/]+)\/risk$/);
    if (riskMatch && method === 'GET') {
      return sendJson(res, 200, {
        overallScore: 84.5,
        classification: "HIGH",
        breakdown: {
          changeSeverityScore: 30.0,
          impactBreadthScore: 23.0,
          dependencyDepthScore: 10.0,
          serviceCriticalityScore: 11.5,
          regressionFailureScore: 10.0,
          severityExplanation: "3 breaking changes (field removed, type mutated, required param added)",
          breadthExplanation: "3 downstream services and 1 client impacted in blast radius",
          depthExplanation: "Max blast radius depth 2 hops",
          criticalityExplanation: "Service criticality rated HIGH",
          regressionExplanation: "2 historical recordings failed due to missing field regression"
        },
        recommendedActions: [
          "BLOCK deployment to production until consumer contracts are migrated",
          "Notify Order Service and Notification Service teams of 'name' -> 'fullName' renaming",
          "Deploy backward-compatible dual-field support (support both 'name' and 'fullName') during transition period"
        ]
      });
    }

    const testsMatch = pathname.match(/^\/api\/v1\/analysis-runs\/([^\/]+)\/tests$/);
    if (testsMatch && method === 'GET') {
      const selected = testCases.filter(t => t.associatedServiceId === 'srv_user' || t.associatedServiceId === 'srv_order' || t.associatedServiceId === 'srv_notification');
      return sendJson(res, 200, {
        mode: "OPTIMIZED",
        totalTestsCount: testCases.length,
        selectedTestsCount: selected.length,
        excludedTestsCount: testCases.length - selected.length,
        selectionPercentage: Math.round((selected.length / testCases.length) * 100),
        estimatedExecutionTimeMs: selected.reduce((acc, t) => acc + t.executionDurationMs, 0),
        totalExecutionTimeSavedMs: testCases.reduce((acc, t) => acc + t.executionDurationMs, 0) - selected.reduce((acc, t) => acc + t.executionDurationMs, 0),
        selectedTests: selected.map(t => ({
          test: t,
          selectionReason: t.associatedServiceId === 'srv_user' ? "Directly tests modified API service" : "Downstream consumer service in blast radius",
          relevanceScore: t.priority === 1 ? 100 : 75
        }))
      });
    }

    // Replay Runs
    if (pathname === '/api/v1/replay-runs' && method === 'GET') {
      return sendJson(res, 200, replayRuns);
    }
    if (pathname === '/api/v1/replay-runs' && method === 'POST') {
      const srv = services.find(s => s.id === body.serviceId) || { name: "User Service" };
      const newReplay = {
        id: `rep_${Date.now()}`,
        analysisRunId: "run_user_v1_to_v2",
        serviceId: body.serviceId || "srv_user",
        serviceName: srv.name,
        targetEnvironment: body.targetEnvironment || "http://localhost:8080",
        status: "COMPLETED",
        totalRequests: 2,
        passedRequests: 0,
        failedRequests: 2,
        createdAt: new Date().toISOString(),
        results: replayRuns[0].results
      };
      replayRuns.unshift(newReplay);
      return sendJson(res, 201, newReplay);
    }

    // Test cases and recordings
    if (pathname === '/api/v1/test-cases' && method === 'GET') {
      return sendJson(res, 200, testCases);
    }
    if (pathname === '/api/v1/recordings' && method === 'GET') {
      return sendJson(res, 200, requestRecordings);
    }
    if (pathname === '/api/v1/specifications' && method === 'GET') {
      return sendJson(res, 200, specifications);
    }

    // GitHub PR Webhook
    if (pathname === '/api/v1/integrations/github/webhook' && method === 'POST') {
      return sendJson(res, 200, {
        status: "PROCESSED",
        pullRequestNumber: body.pullRequest?.number || 42,
        commentPosted: true,
        markdownReport: `### 🔍 TraceWeaver API Change Impact Report
- **Service**: User Service
- **PR**: #${body.pullRequest?.number || 42}
- **Compatibility**: 🚨 **BREAKING** (Score: 84.5/100)
- **Impacted Services**: Order Service, Notification Service, Recommendation Service
- **Impacted Clients**: Mobile App (iOS/Android)
- **Confirmed Regressions**: 2 requests failed historical baseline (Field \`name\` removed)
- **Action**: Deployment **BLOCKED**. Please provide deprecation window or backward-compatible alias.`
      });
    }

    // Fallback 404
    sendJson(res, 404, { error: 'Route not found', path: pathname });
  });
});

server.listen(PORT, () => {
  console.log(`TraceWeaver Server running on http://localhost:${PORT}`);
});
