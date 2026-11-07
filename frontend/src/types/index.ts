export type NodeType = 'SERVICE' | 'API_OPERATION' | 'SCHEMA_OBJECT' | 'SCHEMA_FIELD' | 'CLIENT' | 'TEST_CASE';
export type RelationshipType = 
  | 'SERVICE_DEPENDS_ON_SERVICE'
  | 'OPERATION_USES_SCHEMA'
  | 'SCHEMA_CONTAINS_FIELD'
  | 'CLIENT_CALLS_OPERATION'
  | 'TEST_COVERS_OPERATION'
  | 'OPERATION_CALLS_OPERATION';

export type Compatibility = 'BREAKING' | 'POTENTIALLY_BREAKING' | 'NON_BREAKING' | 'UNKNOWN';
export type Severity = 'INFO' | 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';

export interface ServiceNode {
  id: string;
  type: NodeType;
  name: string;
  description: string;
  metadata?: Record<string, string>;
}

export interface DependencyEdge {
  source: string;
  target: string;
  relationship: RelationshipType;
  weight: number;
}

export interface ApiChange {
  id: string;
  type: string;
  category: string;
  severity: Severity;
  compatibility: Compatibility;
  oldValue: string;
  newValue: string;
  jsonPointer: string;
  operationId: string;
  description: string;
  reason: string;
}

export interface ImpactedComponent {
  id: string;
  nodeId: string;
  componentName: string;
  componentType: NodeType;
  impactLevel: 'DIRECT' | 'INDIRECT' | 'POTENTIAL';
  depth: number;
  dependencyPath: string[];
  reason: string;
}

export interface RiskReport {
  overallScore: number;
  classification: 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
  breakdown: {
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
  };
  recommendedActions: string[];
}

export interface ReplayResult {
  recordingId: string;
  primaryRegression: string;
  isFailure: boolean;
  expectedStatus: number;
  actualStatus: number;
  historicalLatencyMs: number;
  replayLatencyMs: number;
  summaryDiff: string;
}
