import React, { useState } from 'react';
import { Sidebar } from './components/layout/Sidebar';
import { Topbar } from './components/layout/Topbar';
import {
  DEMO_PROJECTS,
  DEMO_SERVICES,
  DEMO_ANALYSIS_RUNS,
  DEMO_CHANGES,
  DEMO_TESTS,
  DEMO_REPLAY_RUN,
  DEMO_GRAPH_DATA,
  DEMO_ACTIVITIES,
  DEMO_RISK_ASSESSMENT,
} from './data/demoData';

export const App: React.FC = () => {
  const [activePage, setActivePage] = useState<string>('overview');
  const [activeProject] = useState(DEMO_PROJECTS[0]);
  const [environment, setEnvironment] = useState<string>('production');
  const [searchQuery, setSearchQuery] = useState<string>('');
  const [analysisSubTab, setAnalysisSubTab] = useState<'summary' | 'changes' | 'tests' | 'replay' | 'breakdown'>('summary');
  const [selectedChangeId, setSelectedChangeId] = useState<string | null>(null);

  const getPrimaryActionLabel = () => {
    switch (activePage) {
      case 'services': return 'Add Service';
      case 'projects': return 'Create Project';
      case 'replay': return 'Start Replay';
      default: return 'Run Analysis';
    }
  };

  const handlePrimaryAction = () => {
    if (activePage === 'replay') {
      alert("Executing Request Replay against staging environment...");
    } else {
      setActivePage('analysis');
    }
  };

  const selectedChange = DEMO_CHANGES.find(c => c.id === selectedChangeId);

  return (
    <div className="flex h-screen bg-[#111827] text-[#F9FAFB] font-sans overflow-hidden">
      {/* Sidebar */}
      <Sidebar
        activePage={activePage}
        onNavigate={setActivePage}
        activeProjectName={activeProject.name}
      />

      {/* Main Content Area */}
      <div className="flex-1 flex flex-col min-w-0 overflow-hidden">
        <Topbar
          projectName={activeProject.name}
          pageTitle={activePage.toUpperCase()}
          environment={environment}
          onEnvChange={setEnvironment}
          primaryActionLabel={getPrimaryActionLabel()}
          onPrimaryAction={handlePrimaryAction}
          searchQuery={searchQuery}
          onSearchChange={setSearchQuery}
        />

        <main className="flex-1 overflow-y-auto p-6 bg-[#111827]">
          {/* PAGE: Overview */}
          {activePage === 'overview' && (
            <div className="space-y-6">
              <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
                <div className="p-4 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <div className="text-[11px] font-medium text-[#9CA3AF]">Total Services</div>
                  <div className="text-2xl font-bold text-white mt-1">7</div>
                  <div className="text-[10px] text-emerald-400 mt-1">&#x25B2; 100% active coverage</div>
                </div>
                <div className="p-4 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <div className="text-[11px] font-medium text-[#9CA3AF]">API Operations</div>
                  <div className="text-2xl font-bold text-white mt-1">28</div>
                  <div className="text-[10px] text-[#9CA3AF] mt-1">Indexed OpenAPI endpoints</div>
                </div>
                <div className="p-4 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <div className="text-[11px] font-medium text-[#9CA3AF]">High-Risk Changes</div>
                  <div className="text-2xl font-bold text-rose-500 mt-1">3</div>
                  <div className="text-[10px] text-rose-400 mt-1">1 field removed, 1 type mutated</div>
                </div>
                <div className="p-4 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <div className="text-[11px] font-medium text-[#9CA3AF]">Failed Regression Tests</div>
                  <div className="text-2xl font-bold text-amber-400 mt-1">2</div>
                  <div className="text-[10px] text-amber-400 mt-1">Confirmed payload mismatches</div>
                </div>
              </div>

              {/* Middle Section */}
              <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
                <div className="lg:col-span-2 p-5 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <h3 className="text-sm font-bold text-white pb-3 border-b border-[#374151]">Recent Analysis Runs</h3>
                  <div className="mt-3 divide-y divide-[#374151]/40 text-xs">
                    {DEMO_ANALYSIS_RUNS.map(run => (
                      <div
                        key={run.id}
                        onClick={() => setActivePage('analysis')}
                        className="py-3 flex items-center justify-between hover:bg-[#111827] px-2 rounded cursor-pointer transition"
                      >
                        <div>
                          <div className="font-bold text-white">{run.serviceName} Analysis</div>
                          <div className="text-[#9CA3AF] font-mono text-[11px]">{run.oldVersion} &rarr; {run.newVersion}</div>
                        </div>
                        <div className="text-right">
                          <span className="bg-rose-500/20 text-rose-400 border border-rose-500/30 px-2 py-0.5 rounded font-mono font-bold">
                            {run.riskScore} (CRITICAL)
                          </span>
                        </div>
                      </div>
                    ))}
                  </div>
                </div>

                <div className="p-5 rounded-xl bg-[#1F2937] border border-[#374151]">
                  <h3 className="text-sm font-bold text-white pb-3 border-b border-[#374151]">Engineering Feed</h3>
                  <div className="mt-3 space-y-3 text-xs">
                    {DEMO_ACTIVITIES.slice(0, 3).map(act => (
                      <div key={act.id} className="space-y-0.5">
                        <div className="font-bold text-white">{act.title}</div>
                        <div className="text-[11px] text-[#9CA3AF]">{act.description}</div>
                        <div className="text-[10px] text-[#6B7280]">{act.timestamp}</div>
                      </div>
                    ))}
                  </div>
                </div>
              </div>
            </div>
          )}

          {/* PAGE: Change Analysis */}
          {activePage === 'analysis' && (
            <div className="space-y-6">
              {/* Header Card */}
              <div className="p-5 rounded-xl bg-[#1F2937] border border-[#374151] flex justify-between items-center">
                <div>
                  <div className="flex items-center space-x-2">
                    <span className="text-xs font-mono font-bold text-white">run_user_v1_to_v2</span>
                    <span className="bg-rose-500/20 text-rose-400 border border-rose-500/30 px-2 py-0.5 rounded text-[10px] font-mono font-bold">
                      CRITICAL / HIGH RISK
                    </span>
                  </div>
                  <h2 className="text-lg font-bold text-white mt-1">User Service Contract Drift Evaluation</h2>
                  <div className="text-xs text-[#9CA3AF] font-mono mt-0.5">
                    User Service &bull; v1.0.0 &rarr; v2.0.0 &bull; Evaluated: 2026-11-08
                  </div>
                </div>
              </div>

              {/* Subtabs */}
              <div className="border-b border-[#374151] flex space-x-6 text-xs font-medium">
                {(['summary', 'changes', 'tests', 'replay', 'breakdown'] as const).map(tab => (
                  <button
                    key={tab}
                    onClick={() => setAnalysisSubTab(tab)}
                    className={`pb-2.5 capitalize transition ${
                      analysisSubTab === tab
                        ? 'text-blue-400 border-b-2 border-blue-500 font-semibold'
                        : 'text-[#9CA3AF] hover:text-white'
                    }`}
                  >
                    {tab}
                  </button>
                ))}
              </div>

              {analysisSubTab === 'summary' && (
                <div className="space-y-4 text-xs">
                  <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
                    <div className="p-4 rounded-xl bg-[#0B1120] border border-amber-500/20">
                      <span className="font-mono font-bold text-amber-400">1. Potential Impact</span>
                      <p className="text-[#9CA3AF] mt-1">Order Service, Notification Service, and Mobile App depend on this API.</p>
                    </div>
                    <div className="p-4 rounded-xl bg-[#0B1120] border border-rose-500/30">
                      <span className="font-mono font-bold text-rose-400">2. Breaking Changes</span>
                      <p className="text-[#9CA3AF] mt-1">Removed field 'name' and mutated field 'tier' type to integer.</p>
                    </div>
                    <div className="p-4 rounded-xl bg-[#0B1120] border border-red-600/40">
                      <span className="font-mono font-bold text-red-400">3. Confirmed Regression</span>
                      <p className="text-[#9CA3AF] mt-1">Replaying 2 production requests against staging failed with missing field errors.</p>
                    </div>
                  </div>
                </div>
              )}

              {analysisSubTab === 'changes' && (
                <div className="p-4 rounded-xl bg-[#1F2937] border border-[#374151] overflow-x-auto text-xs">
                  <table className="w-full text-left">
                    <thead>
                      <tr className="text-[#6B7280] border-b border-[#374151] font-mono uppercase text-[10px]">
                        <th className="py-2">Change</th>
                        <th className="py-2">Operation</th>
                        <th className="py-2">Type</th>
                        <th className="py-2">Compatibility</th>
                        <th className="py-2 text-right">Inspect</th>
                      </tr>
                    </thead>
                    <tbody className="divide-y divide-[#374151]/40">
                      {DEMO_CHANGES.map(c => (
                        <tr key={c.id} onClick={() => setSelectedChangeId(c.id)} className="hover:bg-[#0B1120] cursor-pointer">
                          <td className="py-2.5 font-bold text-white">{c.description}</td>
                          <td className="py-2.5 font-mono text-[#9CA3AF]">{c.operationId}</td>
                          <td className="py-2.5 font-mono text-[10px]">{c.type}</td>
                          <td className="py-2.5 font-mono font-bold text-rose-400">{c.compatibility}</td>
                          <td className="py-2.5 text-right text-blue-400 font-semibold">Inspect &rarr;</td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              )}
            </div>
          )}

          {/* PAGE: Dependency Graph */}
          {activePage === 'graph' && (
            <div className="p-5 rounded-xl bg-[#0B1120] border border-[#374151] h-[550px] flex items-center justify-center font-mono text-xs text-[#9CA3AF]">
              Interactive SVG Graph rendered on canvas. Open http://localhost:4000 to interact with live nodes and blast radius paths.
            </div>
          )}

          {/* Other Pages Fallback Container */}
          {!['overview', 'analysis', 'graph'].includes(activePage) && (
            <div className="p-6 rounded-xl bg-[#1F2937] border border-[#374151] text-xs">
              <h3 className="text-sm font-bold text-white capitalize mb-2">{activePage} Workspace</h3>
              <p className="text-[#9CA3AF]">Connected to TraceWeaver live data layer. All changes, tests, and replay simulations are active.</p>
            </div>
          )}
        </main>
      </div>
    </div>
  );
};

export default App;
