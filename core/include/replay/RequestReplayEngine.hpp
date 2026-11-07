#pragma once

#include "RequestRecording.hpp"
#include "ResponseDiff.hpp"
#include "ResponseComparator.hpp"
#include <vector>
#include <functional>
#include <string>

namespace traceweaver::replay {

struct ReplayRunConfig {
    std::string targetBaseUrl; // e.g. "http://localhost:8080"
    int timeoutMs{5000};
    int maxRetries{2};
    size_t concurrencyLimit{5};
    bool allowStateChangingReplay{false}; // safeguard for POST/PUT/DELETE
};

struct ReplayRunSummary {
    std::string replayRunId;
    std::string targetBaseUrl;
    size_t totalRequests{0};
    size_t passedRequests{0};
    size_t failedRequests{0};
    size_t skippedRequests{0};
    double totalDurationMs{0.0};
    std::vector<ResponseComparisonResult> results;
};

// Simulation callback or real HTTP client executor
using HttpExecutor = std::function<std::pair<int, std::string>(
    const std::string& method,
    const std::string& fullUrl,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& body)>;

class RequestReplayEngine {
public:
    explicit RequestReplayEngine(ReplayRunConfig config, ResponseComparator comparator = ResponseComparator{})
        : config_(std::move(config)), comparator_(std::move(comparator)) {}

    [[nodiscard]] ReplayRunSummary executeReplay(
        const std::vector<RequestRecording>& recordings,
        HttpExecutor executor = nullptr);

private:
    ReplayRunConfig config_;
    ResponseComparator comparator_;
};

} // namespace traceweaver::replay
