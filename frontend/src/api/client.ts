// TraceWeaver Centralized API Client
import {
  Project,
  Service,
  ApiSpecification,
  AnalysisRun,
  ApiChange,
  ImpactedComponent,
  RiskAssessment,
  TestCase,
  ReplayRun,
  DependencyGraphData,
} from '../types';
import {
  DEMO_PROJECTS,
  DEMO_SERVICES,
  DEMO_ANALYSIS_RUNS,
  DEMO_CHANGES,
  DEMO_IMPACTED_COMPONENTS,
  DEMO_RISK_ASSESSMENT,
  DEMO_TESTS,
  DEMO_REPLAY_RUN,
  DEMO_GRAPH_DATA,
} from '../data/demoData';

const BASE_URL = '/api/v1';

class ApiClient {
  private isLiveBackend: boolean | null = null;

  private async request<T>(endpoint: string, options?: RequestInit, fallbackData?: T): Promise<T> {
    try {
      const response = await fetch(`${BASE_URL}${endpoint}`, {
        ...options,
        headers: {
          'Content-Type': 'application/json',
          ...(options?.headers || {}),
        },
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      this.isLiveBackend = true;
      return await response.json();
    } catch (error) {
      this.isLiveBackend = false;
      if (fallbackData !== undefined) {
        return fallbackData;
      }
      throw error;
    }
  }

  public getBackendStatus(): boolean {
    return this.isLiveBackend === true;
  }

  // Health
  async checkHealth(): Promise<{ status: string; engine: string }> {
    return this.request('/health', { method: 'GET' }, { status: 'UP', engine: 'TraceWeaver C++20 Core Bridge (Fallback)' });
  }

  // Projects
  async getProjects(): Promise<Project[]> {
    return this.request('/projects', { method: 'GET' }, DEMO_PROJECTS);
  }

  async createProject(project: Partial<Project>): Promise<Project> {
    const fallback: Project = {
      id: `proj_${Date.now()}`,
      workspaceId: 'ws_default',
      name: project.name || 'New Project',
      slug: (project.name || 'new-project').toLowerCase().replace(/\s+/g, '-'),
      description: project.description || '',
      defaultEnvironment: project.defaultEnvironment || 'production',
      servicesCount: 0,
      operationsCount: 0,
      createdAt: new Date().toISOString(),
    };
    return this.request('/projects', { method: 'POST', body: JSON.stringify(project) }, fallback);
  }

  // Services
  async getServices(projectId = 'proj_ecommerce'): Promise<Service[]> {
    return this.request(`/projects/${projectId}/services`, { method: 'GET' }, DEMO_SERVICES);
  }

  async createService(service: Partial<Service>): Promise<Service> {
    const fallback: Service = {
      id: `srv_${Date.now()}`,
      projectId: 'proj_ecommerce',
      name: service.name || 'New Service',
      description: service.description || '',
      repositoryUrl: service.repositoryUrl || '',
      environment: service.environment || 'production',
      criticality: service.criticality || 'MEDIUM',
      specVersion: '1.0.0',
      operationsCount: 0,
      directDependenciesCount: 0,
      downstreamConsumersCount: 0,
      status: 'HEALTHY',
    };
    return this.request(`/projects/${service.projectId || 'proj_ecommerce'}/services`, {
      method: 'POST',
      body: JSON.stringify(service),
    }, fallback);
  }

  // Specifications
  async getSpecifications(serviceId = 'srv_user'): Promise<ApiSpecification[]> {
    const fallback: ApiSpecification[] = [
      {
        id: 'spec_user',
        serviceId: 'srv_user',
        serviceName: 'User Service',
        title: 'User Service OpenAPI Spec',
        currentVersion: '2.0.0',
        specType: 'OPENAPI_3',
        uploadDate: '2026-11-08T10:00:00Z',
        validationStatus: 'WARNINGS',
        totalEndpoints: 6,
        totalSchemas: 4,
        parsingErrors: [],
        warnings: ["Response property 'name' removed in v2.0.0 (breaking change)"],
        unsupportedFeatures: ['OpenAPI 3 Callbacks are ignored in static analysis'],
        versions: [
          { versionTag: '1.0.0', createdAt: '2026-10-15T08:00:00Z', rawContent: '{}' },
          { versionTag: '2.0.0', createdAt: '2026-11-08T10:00:00Z', rawContent: '{}' },
        ],
      },
    ];
    return this.request(`/services/${serviceId}/specifications`, { method: 'GET' }, fallback);
  }

  // Dependency Graph
  async getDependencyGraph(projectId = 'proj_ecommerce'): Promise<DependencyGraphData> {
    return this.request(`/projects/${projectId}/dependency-graph`, { method: 'GET' }, DEMO_GRAPH_DATA);
  }

  // Analysis Runs
  async getAnalysisRuns(): Promise<AnalysisRun[]> {
    return this.request('/analysis-runs', { method: 'GET' }, DEMO_ANALYSIS_RUNS);
  }

  async getAnalysisChanges(runId = 'run_user_v1_to_v2'): Promise<ApiChange[]> {
    return this.request(`/analysis-runs/${runId}/changes`, { method: 'GET' }, DEMO_CHANGES);
  }

  async getAnalysisImpact(runId = 'run_user_v1_to_v2'): Promise<ImpactedComponent[]> {
    return this.request(`/analysis-runs/${runId}/impact`, { method: 'GET' }, DEMO_IMPACTED_COMPONENTS);
  }

  async getAnalysisRisk(runId = 'run_user_v1_to_v2'): Promise<RiskAssessment> {
    return this.request(`/analysis-runs/${runId}/risk`, { method: 'GET' }, DEMO_RISK_ASSESSMENT);
  }

  async getAnalysisTests(runId = 'run_user_v1_to_v2'): Promise<{ selectedTests: any[]; totalTestsCount: number; selectedTestsCount: number }> {
    const fallback = {
      selectedTests: DEMO_TESTS.filter(t => t.isSelected).map(t => ({ test: t, selectionReason: t.selectionReason, relevanceScore: t.priority === 1 ? 100 : 75 })),
      totalTestsCount: DEMO_TESTS.length,
      selectedTestsCount: 4,
    };
    return this.request(`/analysis-runs/${runId}/tests`, { method: 'GET' }, fallback);
  }

  async triggerAnalysis(serviceId: string, oldVersion: string, newVersion: string): Promise<AnalysisRun> {
    const fallback: AnalysisRun = {
      id: `run_${Date.now()}`,
      projectId: 'proj_ecommerce',
      serviceId,
      serviceName: 'User Service',
      oldVersion,
      newVersion,
      status: 'COMPLETED',
      riskScore: 84.5,
      riskClassification: 'CRITICAL',
      createdAt: new Date().toISOString(),
      breakingChangesCount: 3,
      potentiallyBreakingCount: 1,
      nonBreakingCount: 1,
      impactedServicesCount: 3,
      impactedClientsCount: 1,
      selectedTestsCount: 4,
      totalTestsCount: 6,
      confirmedRegressionsCount: 2,
    };
    return this.request('/analysis-runs', {
      method: 'POST',
      body: JSON.stringify({ serviceId, oldVersion, newVersion }),
    }, fallback);
  }

  // Regression Tests
  async getTestCases(): Promise<TestCase[]> {
    return this.request('/test-cases', { method: 'GET' }, DEMO_TESTS);
  }

  // Replay Runs
  async getReplayRuns(): Promise<ReplayRun[]> {
    return this.request('/replay-runs', { method: 'GET' }, [DEMO_REPLAY_RUN]);
  }

  async triggerReplay(serviceId: string, targetEnvironment: string): Promise<ReplayRun> {
    const fallback: ReplayRun = {
      ...DEMO_REPLAY_RUN,
      id: `rep_${Date.now()}`,
      createdAt: new Date().toISOString(),
      targetEnvironment,
    };
    return this.request('/replay-runs', {
      method: 'POST',
      body: JSON.stringify({ serviceId, targetEnvironment }),
    }, fallback);
  }
}

export const api = new ApiClient();
