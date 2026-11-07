#pragma once

#include "Change.hpp"
#include <vector>
#include <string>

namespace traceweaver::diff {

struct DiffReport {
    std::string oldVersion;
    std::string newVersion;
    std::vector<ApiChange> changes;
    
    // Aggregated statistics
    size_t breakingCount{0};
    size_t potentiallyBreakingCount{0};
    size_t nonBreakingCount{0};
    size_t totalChanges{0};

    [[nodiscard]] bool hasBreakingChanges() const noexcept {
        return breakingCount > 0;
    }
};

} // namespace traceweaver::diff
