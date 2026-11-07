-- TraceWeaver Production Database Schema Migration
-- Version: 001_initial_schema.sql

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Workspaces
CREATE TABLE IF NOT EXISTS workspaces (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    slug VARCHAR(255) UNIQUE NOT NULL,
    description TEXT,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Workspace Members
CREATE TABLE IF NOT EXISTS workspace_members (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    workspace_id UUID NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,
    user_id UUID NOT NULL,
    role VARCHAR(50) NOT NULL DEFAULT 'DEVELOPER', -- OWNER, ADMIN, DEVELOPER, VIEWER
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uq_workspace_member UNIQUE (workspace_id, user_id)
);

-- Projects
CREATE TABLE IF NOT EXISTS projects (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    workspace_id UUID NOT NULL REFERENCES workspaces(id) ON DELETE CASCADE,
    name VARCHAR(255) NOT NULL,
    slug VARCHAR(255) NOT NULL,
    description TEXT,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uq_project_slug UNIQUE (workspace_id, slug)
);

-- Services
CREATE TABLE IF NOT EXISTS services (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    project_id UUID NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    repository_url VARCHAR(512),
    environment VARCHAR(50) DEFAULT 'production',
    criticality VARCHAR(50) DEFAULT 'MEDIUM', -- LOW, MEDIUM, HIGH, CRITICAL
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- API Specifications
CREATE TABLE IF NOT EXISTS api_specifications (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    title VARCHAR(255) NOT NULL,
    current_version VARCHAR(50) NOT NULL,
    spec_type VARCHAR(50) DEFAULT 'OPENAPI_3',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Specification Versions
CREATE TABLE IF NOT EXISTS api_specification_versions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    specification_id UUID NOT NULL REFERENCES api_specifications(id) ON DELETE CASCADE,
    version_tag VARCHAR(50) NOT NULL,
    raw_content TEXT NOT NULL,
    content_format VARCHAR(20) DEFAULT 'JSON', -- JSON, YAML
    parsed_metadata JSONB,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- API Operations
CREATE TABLE IF NOT EXISTS api_operations (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    operation_id VARCHAR(255) NOT NULL,
    http_method VARCHAR(20) NOT NULL,
    path VARCHAR(512) NOT NULL,
    summary TEXT,
    request_parameters JSONB,
    request_body_schema JSONB,
    response_schemas JSONB,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Dependency Nodes
CREATE TABLE IF NOT EXISTS dependency_nodes (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    project_id UUID NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    node_key VARCHAR(255) NOT NULL,
    node_type VARCHAR(50) NOT NULL, -- SERVICE, API_OPERATION, SCHEMA_OBJECT, SCHEMA_FIELD, CLIENT, TEST_CASE
    name VARCHAR(255) NOT NULL,
    description TEXT,
    metadata JSONB,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uq_project_node_key UNIQUE (project_id, node_key)
);

-- Dependency Edges
CREATE TABLE IF NOT EXISTS dependency_edges (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    project_id UUID NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    source_node_id UUID NOT NULL REFERENCES dependency_nodes(id) ON DELETE CASCADE,
    destination_node_id UUID NOT NULL REFERENCES dependency_nodes(id) ON DELETE CASCADE,
    relationship_type VARCHAR(50) NOT NULL,
    weight DOUBLE PRECISION DEFAULT 1.0,
    metadata JSONB,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Analysis Runs
CREATE TABLE IF NOT EXISTS analysis_runs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    project_id UUID NOT NULL REFERENCES projects(id) ON DELETE CASCADE,
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    old_version_id UUID REFERENCES api_specification_versions(id),
    new_version_id UUID REFERENCES api_specification_versions(id),
    status VARCHAR(50) DEFAULT 'QUEUED', -- QUEUED, RUNNING, COMPLETED, FAILED
    risk_score DOUBLE PRECISION,
    risk_classification VARCHAR(50),
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP WITH TIME ZONE
);

-- Detected Changes
CREATE TABLE IF NOT EXISTS detected_changes (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    analysis_run_id UUID NOT NULL REFERENCES analysis_runs(id) ON DELETE CASCADE,
    change_type VARCHAR(100) NOT NULL,
    category VARCHAR(50) NOT NULL,
    severity VARCHAR(50) NOT NULL,
    compatibility VARCHAR(50) NOT NULL,
    json_pointer VARCHAR(512),
    operation_id VARCHAR(255),
    description TEXT,
    reason TEXT,
    old_value TEXT,
    new_value TEXT
);

-- Impacted Components
CREATE TABLE IF NOT EXISTS impacted_components (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    analysis_run_id UUID NOT NULL REFERENCES analysis_runs(id) ON DELETE CASCADE,
    node_id UUID REFERENCES dependency_nodes(id) ON DELETE SET NULL,
    component_name VARCHAR(255) NOT NULL,
    component_type VARCHAR(50) NOT NULL,
    impact_level VARCHAR(50) NOT NULL,
    depth INTEGER DEFAULT 1,
    dependency_path TEXT[],
    reason TEXT
);

-- Test Cases
CREATE TABLE IF NOT EXISTS test_cases (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    test_identifier VARCHAR(255) NOT NULL,
    name VARCHAR(255) NOT NULL,
    test_type VARCHAR(50) DEFAULT 'REGRESSION',
    associated_operation_id VARCHAR(255),
    priority INTEGER DEFAULT 1,
    duration_ms DOUBLE PRECISION DEFAULT 100.0,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Request Recordings
CREATE TABLE IF NOT EXISTS request_recordings (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    operation_id VARCHAR(255),
    environment VARCHAR(50) DEFAULT 'production',
    http_method VARCHAR(20) NOT NULL,
    url_path VARCHAR(512) NOT NULL,
    query_parameters JSONB,
    request_headers JSONB,
    request_body TEXT,
    response_status INTEGER NOT NULL,
    response_headers JSONB,
    response_body TEXT,
    latency_ms DOUBLE PRECISION,
    is_sanitized BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Replay Runs
CREATE TABLE IF NOT EXISTS replay_runs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    analysis_run_id UUID REFERENCES analysis_runs(id) ON DELETE CASCADE,
    service_id UUID NOT NULL REFERENCES services(id) ON DELETE CASCADE,
    target_environment VARCHAR(255) NOT NULL,
    status VARCHAR(50) DEFAULT 'QUEUED',
    total_requests INTEGER DEFAULT 0,
    passed_requests INTEGER DEFAULT 0,
    failed_requests INTEGER DEFAULT 0,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP WITH TIME ZONE
);

-- Background Jobs
CREATE TABLE IF NOT EXISTS background_jobs (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    job_type VARCHAR(100) NOT NULL,
    status VARCHAR(50) DEFAULT 'QUEUED',
    payload JSONB,
    attempts INTEGER DEFAULT 0,
    max_attempts INTEGER DEFAULT 3,
    error_message TEXT,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Indexes for high-performance querying
CREATE INDEX IF NOT EXISTS idx_services_project_id ON services(project_id);
CREATE INDEX IF NOT EXISTS idx_nodes_project_id ON dependency_nodes(project_id);
CREATE INDEX IF NOT EXISTS idx_edges_source ON dependency_edges(source_node_id);
CREATE INDEX IF NOT EXISTS idx_edges_dest ON dependency_edges(destination_node_id);
CREATE INDEX IF NOT EXISTS idx_analysis_runs_project ON analysis_runs(project_id);
CREATE INDEX IF NOT EXISTS idx_detected_changes_run ON detected_changes(analysis_run_id);
CREATE INDEX IF NOT EXISTS idx_recordings_service ON request_recordings(service_id);
