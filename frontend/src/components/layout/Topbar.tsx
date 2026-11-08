import React from 'react';

interface TopbarProps {
  projectName: string;
  pageTitle: string;
  environment: string;
  onEnvChange: (env: string) => void;
  primaryActionLabel: string;
  onPrimaryAction: () => void;
  searchQuery: string;
  onSearchChange: (q: string) => void;
}

export const Topbar: React.FC<TopbarProps> = ({
  projectName,
  pageTitle,
  environment,
  onEnvChange,
  primaryActionLabel,
  onPrimaryAction,
  searchQuery,
  onSearchChange,
}) => {
  return (
    <header className="h-14 border-b border-[#374151] bg-[#111827]/95 backdrop-blur px-6 flex items-center justify-between flex-shrink-0 z-20">
      {/* Breadcrumbs */}
      <div className="flex items-center space-x-2 text-xs font-medium">
        <span className="text-[#9CA3AF]">{projectName}</span>
        <span className="text-[#4B5563]">/</span>
        <span className="text-[#F9FAFB] font-semibold">{pageTitle}</span>
      </div>

      {/* Controls */}
      <div className="flex items-center space-x-3">
        {/* Environment Selector */}
        <div className="flex items-center space-x-1.5 px-2.5 py-1 rounded-md bg-[#1F2937] border border-[#374151] text-xs">
          <span className="w-1.5 h-1.5 rounded-full bg-emerald-500"></span>
          <select
            value={environment}
            onChange={(e) => onEnvChange(e.target.value)}
            className="bg-transparent text-[#F9FAFB] text-[11px] font-mono font-medium focus:outline-none cursor-pointer"
          >
            <option value="production">production</option>
            <option value="staging">staging</option>
            <option value="sandbox">sandbox</option>
          </select>
        </div>

        {/* Global Search */}
        <div className="relative w-48 lg:w-64">
          <input
            type="text"
            value={searchQuery}
            onChange={(e) => onSearchChange(e.target.value)}
            placeholder="Search operations, IDs..."
            className="w-full bg-[#1F2937] text-xs text-[#F9FAFB] placeholder-[#6B7280] rounded-md px-3 py-1.5 border border-[#374151] focus:outline-none focus:border-blue-500 transition"
          />
        </div>

        {/* Primary Action Button */}
        <button
          onClick={onPrimaryAction}
          className="flex items-center space-x-1.5 px-3 py-1.5 rounded-md bg-[#3B82F6] hover:bg-blue-600 active:scale-95 text-xs font-semibold text-white shadow-sm transition"
        >
          <span>{primaryActionLabel}</span>
        </button>
      </div>
    </header>
  );
};
