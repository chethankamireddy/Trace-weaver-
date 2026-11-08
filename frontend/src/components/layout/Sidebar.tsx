import React from 'react';

interface SidebarProps {
  activePage: string;
  onNavigate: (page: string) => void;
  activeProjectName: string;
}

export const Sidebar: React.FC<SidebarProps> = ({ activePage, onNavigate, activeProjectName }) => {
  return (
    <aside className="w-60 flex-shrink-0 bg-[#0B1120] border-r border-[#374151] flex flex-col justify-between select-none">
      <div className="flex flex-col h-full">
        {/* Brand Header */}
        <div className="p-4 border-b border-[#374151]/80">
          <div className="flex items-center space-x-2.5">
            <div className="w-7 h-7 rounded-lg bg-gradient-to-tr from-blue-600 to-indigo-500 flex items-center justify-center font-mono font-bold text-xs text-white shadow-md shadow-blue-500/20">
              TW
            </div>
            <div className="flex-1 min-w-0">
              <div className="text-[13px] font-bold tracking-tight text-white truncate">TraceWeaver</div>
              <div className="text-[10px] text-[#9CA3AF] font-mono truncate">API Impact Platform</div>
            </div>
          </div>

          <div className="mt-3">
            <label className="text-[10px] uppercase font-semibold text-[#6B7280] tracking-wider block mb-1">
              Active Project
            </label>
            <div className="bg-[#1F2937] text-xs text-white font-medium border border-[#374151] rounded-lg px-2.5 py-1.5 truncate">
              {activeProjectName}
            </div>
          </div>
        </div>

        {/* Navigation Sections */}
        <nav className="flex-1 overflow-y-auto px-2 py-3 space-y-4 text-xs font-medium">
          
          {/* WORKSPACE */}
          <div>
            <span className="px-2.5 text-[10px] uppercase tracking-wider font-semibold text-[#6B7280]">
              Workspace
            </span>
            <div className="mt-1 space-y-0.5">
              {[
                { id: 'overview', label: 'Overview' },
                { id: 'projects', label: 'Projects' },
                { id: 'services', label: 'Services' },
              ].map(item => (
                <button
                  key={item.id}
                  onClick={() => onNavigate(item.id)}
                  className={`w-full flex items-center space-x-2.5 px-3 py-2 rounded-lg transition ${
                    activePage === item.id
                      ? 'bg-blue-600/15 text-blue-400 font-semibold border-l-2 border-blue-500'
                      : 'text-[#9CA3AF] hover:text-white hover:bg-[#1F2937]/50'
                  }`}
                >
                  <span>{item.label}</span>
                </button>
              ))}
            </div>
          </div>

          {/* ANALYSIS */}
          <div>
            <span className="px-2.5 text-[10px] uppercase tracking-wider font-semibold text-[#6B7280]">
              Analysis
            </span>
            <div className="mt-1 space-y-0.5">
              {[
                { id: 'analysis', label: 'Change Analysis', badge: '84.5' },
                { id: 'graph', label: 'Dependency Graph' },
                { id: 'tests', label: 'Regression Tests' },
                { id: 'replay', label: 'Request Replay' },
                { id: 'reports', label: 'Risk Reports' },
              ].map(item => (
                <button
                  key={item.id}
                  onClick={() => onNavigate(item.id)}
                  className={`w-full flex items-center justify-between px-3 py-2 rounded-lg transition ${
                    activePage === item.id
                      ? 'bg-blue-600/15 text-blue-400 font-semibold border-l-2 border-blue-500'
                      : 'text-[#9CA3AF] hover:text-white hover:bg-[#1F2937]/50'
                  }`}
                >
                  <span>{item.label}</span>
                  {item.badge && (
                    <span className="bg-rose-500/20 text-rose-400 border border-rose-500/30 px-1.5 py-0.2 rounded text-[9px] font-mono font-bold">
                      {item.badge}
                    </span>
                  )}
                </button>
              ))}
            </div>
          </div>

          {/* CONFIGURATION */}
          <div>
            <span className="px-2.5 text-[10px] uppercase tracking-wider font-semibold text-[#6B7280]">
              Configuration
            </span>
            <div className="mt-1 space-y-0.5">
              {[
                { id: 'specifications', label: 'API Specifications' },
                { id: 'integrations', label: 'Integrations' },
                { id: 'settings', label: 'Settings' },
              ].map(item => (
                <button
                  key={item.id}
                  onClick={() => onNavigate(item.id)}
                  className={`w-full flex items-center space-x-2.5 px-3 py-2 rounded-lg transition ${
                    activePage === item.id
                      ? 'bg-blue-600/15 text-blue-400 font-semibold border-l-2 border-blue-500'
                      : 'text-[#9CA3AF] hover:text-white hover:bg-[#1F2937]/50'
                  }`}
                >
                  <span>{item.label}</span>
                </button>
              ))}
            </div>
          </div>

        </nav>

        {/* Footer Profile */}
        <div className="p-3 border-t border-[#374151]/80 bg-[#070b14] flex items-center justify-between text-xs">
          <div className="flex items-center space-x-2 min-w-0">
            <div className="w-6 h-6 rounded-full bg-blue-600/30 border border-blue-500/40 flex items-center justify-center text-[10px] font-bold text-blue-400">
              CR
            </div>
            <div className="min-w-0">
              <div className="text-[11px] font-semibold text-white truncate">Chethan Reddy</div>
              <div className="text-[9px] text-[#9CA3AF] truncate">chethan@traceweaver.local</div>
            </div>
          </div>
        </div>
      </div>
    </aside>
  );
};
