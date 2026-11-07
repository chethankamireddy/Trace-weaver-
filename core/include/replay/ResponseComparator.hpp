#pragma once

#include "ResponseDiff.hpp"
#include "RequestRecording.hpp"
#include <unordered_set>
#include <vector>
#include <string>

namespace traceweaver::replay {

struct ComparisonConfig {
    std::unordered_set<std::string> ignoredFieldKeys{
        "timestamp", "time", "date", "createdAt", "updatedAt",
        "requestId", "traceId", "spanId", "idempotencyKey", "nonce",
        "token", "jwt", "etag"
    };
    double latencyThresholdMs{300.0};
    double latencyPercentageTolerance{50.0}; // 50% slower before flagging performance regression
};

class ResponseComparator {
public:
    explicit ResponseComparator(ComparisonConfig config = ComparisonConfig{})
        : config_(std::move(config)) {}

    [[nodiscard]] ResponseComparisonResult compareResponses(
        const RequestRecording& historical,
        int replayStatusCode,
        const std::unordered_map<std::string, std::string>& replayHeaders,
        const std::string& replayBody,
        double replayLatencyMs);

private:
    ComparisonConfig config_;
};

} // namespace traceweaver::replay
