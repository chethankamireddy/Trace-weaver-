import React, { useState, useEffect } from 'react';
import { ApiChange, ImpactedComponent, ReplayResult, ServiceNode } from './types';

export const App: React.FC = () => {
  const [activeTab, setActiveTab] = useState<'overview' | 'services' | 'graph' | 'diff' | 'risk' | 'tests' | 'replay' | 'github'>('overview');
  const [changes, setChanges] = useState<ApiChange[]>([]);
  const [riskScore, setRiskScore] = useState<number>(84.5);

  return (
    <div className="min-h-screen bg-slate-950 text-slate-100 flex flex-col font-sans">
      {/* Header */}
      <header className="border-b border-slate-800 bg-slate-900/80 backdrop-blur sticky top-0 z-50 px-6 py-4 flex items-center justify-between">
        <div className="flex items-center space-x-3">
          <div className="w-8 h-8 rounded-lg bg-indigo-600 flex items-center justify-center font-bold text-white shadow-lg shadow-indigo-500/30">
            TW
          </div>
          <span className="font-extrabold text-lg tracking-tight">TraceWeaver</span>
          <span className="text-xs font-mono font-medium px-2 py-0.5 rounded bg-indigo-950 text-indigo-400 border border-indigo-800">
            C++20 Engine
          </span>
        </div>
        <div className="flex items-center space-x-3">
          <span className="w-2 h-2 rounded-full bg-emerald-500 animate-pulse"></span>
          <span className="text-xs text-slate-300">Engine Operational</span>
        </div>
      </header>

      {/* Main Layout */}
      <div className="flex-1 flex max-w-7xl w-full mx-auto p-6 gap-6">
        <aside className="w-64 space-y-1">
          {(['overview', 'services', 'graph', 'diff', 'risk', 'tests', 'replay', 'github'] as const).map(tab => (
            <button
              key={tab}
              onClick={() => setActiveTab(tab)}
              className={`w-full text-left px-4 py-2.5 rounded-xl text-xs font-semibold capitalize transition ${
                activeTab === tab
                  ? 'bg-indigo-600 text-white shadow-sm shadow-indigo-600/30'
                  : 'text-slate-400 hover:text-slate-200 hover:bg-slate-900'
              }`}
            >
              {tab.replace('-', ' ')}
            </button>
          ))}
        </aside>

        <main className="flex-1 bg-slate-900/60 p-6 rounded-2xl border border-slate-800">
          <h2 className="text-xl font-bold capitalize text-white mb-2">{activeTab}</h2>
          <p className="text-xs text-slate-400 mb-6">Real-time API contract impact analysis and regression replay workspace</p>
          
          <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 text-xs text-slate-300">
            Interactive dashboard active. Switch between tabs to inspect dependency graph, contract diffs, risk scoring, and replay logs.
          </div>
        </main>
      </div>
    </div>
  );
};

export default App;
