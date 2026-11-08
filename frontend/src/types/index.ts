// TraceWeaver Complete Type System

export type Environment = 'production' | 'staging' | 'sandbox' | 'development';
export type Criticality = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
export type RiskLevel = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
export type Compatibility = 'BREAKING' | 'POTENTIALLY_BREAKING' | 'NON_BREAKING' | 'UNKNOWN';
export type ChangeSeverity = 'INFO' | 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';

export type NodeType = 'SERVICE' | 'API_OPERATION' | 'SCHEMA_OBJECT' | 'SCHEMA_FIELD' | 'CLIENT' | 'TEST_CASE';
export type RelationshipType = 
  | 'SERVICE_DEPENDS_ON_SERVICE'
  | 'OPERATION_USES_SCHEMA'
  | 'SCHEMA_CONTAINS_FIELD'
  | 'CLIENT_CALLS_OPERATION'
  | 'TEST_COVERS_OPERATION'
  | 'OPERATION_CALLS_OPERATION';

export type TestType = 'UNIT' | 'INTEGRATION' | 'CONTRACT' | 'REGRESSION' | 'END_TO_END';
export type RegressionType = 
  | 'NO_REGRESSION'
  | 'STATUS_REGRESSION'
  | 'STRUCTURAL_REGRESSION'
  | 'VALUE_REGRESSION'
  | 'HEADER_REGRESSION'
  | 'PERFORMANCE_REGRESSION';

export interface Workspace {
  id: string;
  name: string;
  slug: string;
  description: string;
  createdAt: string;
}

export interface Project {
  id: string;
  workspaceId: string;
  name: string;
  slug: string;
  description: string;
  repositoryUrl?: string;
  defaultEnvironment: Environment;
  servicesCount: number;
  operationsCount: number;
  latestAnalysisDate?: string;
  latestRiskLevel?: RiskLevel;
  createdAt: string;
}

export interface Service {
  id: string;
  projectId: string;
  name: string;
  description: string;
  repositoryUrl: string;
  environment: Environment;
  criticality: Criticality;
  specVersion: string;
  operationsCount: number;
  directDependenciesCount: number;
  downstreamConsumersCount: number;
  status: 'HEALTHY' | 'DEGRADED' | 'BREAKING_CHANGES';
  testEnvironmentBaseUrl?: string;
}

export interface ApiOperation {
  id: string;
  serviceId: string;
  method: 'GET' | 'POST' | 'PUT' | 'PATCH' | 'DELETE';
  path: string;
  operationId: string;
  summary: string;
  description?: string;
  requestSchema?: any;
  responseSchema?: any;
  parameters?: Array<{ name: string; in: string; required: boolean; type: string }>;
}

export interface ApiSpecification {
  id: string;
  serviceId: string;
  serviceName: string;
  title: string;
  currentVersion: string;
  specType: 'OPENAPI_3' | 'OPENAPI_3_1';
  uploadDate: string;
  validationStatus: 'VALID' | 'WARNINGS' | 'ERRORS';
  totalEndpoints: number;
  totalSchemas: number;
  parsingErrors: string[];
  warnings: string[];
  unsupportedFeatures: string[];
  versions: Array<{ versionTag: string; createdAt: string; rawContent: string }>;
}

export interface ApiChange {
  id: string;
  analysisRunId: string;
  type: string;
  category: 'ENDPOINT' | 'REQUEST_PARAMETER' | 'REQUEST_BODY' | 'RESPONSE_STATUS' | 'RESPONSE_FIELD';
  severity: ChangeSeverity;
  compatibility: Compatibility;
  oldValue: string;
  newValue: string;
  jsonPointer: string;
  operationId: string;
  description: string;
  reason: string;
  impactedConsumers?: string[];
  relatedTests?: string[];
}

export interface ImpactedComponent {
  id: string;
  analysisRunId: string;
  nodeId: string;
  componentName: string;
  componentType: NodeType;
  impactLevel: 'DIRECT' | 'INDIRECT' | 'POTENTIAL';
  depth: number;
  dependencyPath: string[];
  reason: string;
}

export interface RiskFactorBreakdown {
  changeSeverityScore: number;
  impactBreadthScore: number;
  dependencyDepthScore: number;
  serviceCriticalityScore: number;
  regressionFailureScore: number;
  severityExplanation: string;
  breadthExplanation: string;
  depthExplanation: string;
  criticalityExplanation: string;
  regressionExplanation: string;
}

export interface RiskAssessment {
  overallScore: number;
  classification: RiskLevel;
  breakdown: RiskFactorBreakdown;
  recommendedActions: string[];
}

export interface TestCase {
  testId: string;
  name: string;
  description: string;
  associatedServiceId: string;
  associatedServiceName: string;
  associatedOperationId: string;
  relatedSchemaFields: string[];
  type: TestType;
  priority: 1 | 2 | 3 | 4 | 5;
  executionDurationMs: number;
  lastExecutionResult: 'PASSED' | 'FAILED' | 'SKIPPED';
  selectionReason?: string;
  isSelected?: boolean;
}

export interface RequestRecording {
  recordingId: string;
  serviceId: string;
  operationId: string;
  environment: Environment;
  httpMethod: string;
  urlPath: string;
  queryParameters?: Record<string, string>;
  requestHeaders: Record<string, string>;
  requestBody: string;
  responseStatus: number;
  responseHeaders: Record<string, string>;
  responseBody: string;
  latencyMs: number;
  isSanitized: boolean;
  recordedAt: string;
}

export interface FieldDifference {
  jsonPointer: string;
  differenceType: 'MISSING_FIELD' | 'EXTRA_FIELD' | 'TYPE_MISMATCH' | 'VALUE_MISMATCH';
  expectedValue: string;
  actualValue: string;
  isIgnored?: boolean;
}

export interface ReplayResult {
  recordingId: string;
  primaryRegression: RegressionType;
  isFailure: boolean;
  expectedStatus: number;
  actualStatus: number;
  historicalLatencyMs: number;
  replayLatencyMs: number;
  summaryDiff: string;
  fieldDifferences: FieldDifference[];
}

export interface ReplayRun {
  id: string;
  analysisRunId: string;
  serviceId: string;
  serviceName: string;
  targetEnvironment: string;
  status: 'QUEUED' | 'RUNNING' | 'COMPLETED' | 'FAILED' | 'CANCELLED';
  totalRequests: number;
  passedRequests: number;
  failedRequests: number;
  durationMs: number;
  createdAt: string;
  results: ReplayResult[];
}

export interface AnalysisRun {
  id: string;
  projectId: string;
  serviceId: string;
  serviceName: string;
  oldVersion: string;
  newVersion: string;
  status: 'QUEUED' | 'RUNNING' | 'COMPLETED' | 'FAILED';
  riskScore: number;
  riskClassification: RiskLevel;
  createdAt: string;
  completedAt?: string;
  breakingChangesCount: number;
  potentiallyBreakingCount: number;
  nonBreakingCount: number;
  impactedServicesCount: number;
  impactedClientsCount: number;
  selectedTestsCount: number;
  totalTestsCount: number;
  confirmedRegressionsCount: number;
}

export interface DependencyGraphData {
  nodes: Array<{
    id: string;
    type: NodeType;
    name: string;
    description?: string;
    criticality?: Criticality;
    metadata?: Record<string, any>;
    x?: number;
    y?: number;
    impacted?: boolean;
    isRootChange?: boolean;
  }>;
  edges: Array<{
    id?: string;
    source: string;
    target: string;
    relationship: RelationshipType;
    weight?: number;
    confidence?: string;
    impacted?: boolean;
  }>;
}

export interface ActivityEvent {
  id: string;
  type: 'SPEC_UPLOAD' | 'ANALYSIS_COMPLETED' | 'REPLAY_FAILED' | 'SERVICE_ADDED' | 'GITHUB_PR';
  title: string;
  description: string;
  timestamp: string;
  severity?: 'INFO' | 'WARNING' | 'DANGER' | 'SUCCESS';
}
