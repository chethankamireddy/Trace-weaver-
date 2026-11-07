#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace traceweaver::replay {

struct RequestRecording {
    std::string recordingId;
    std::string serviceId;
    std::string operationId;
    std::string environment{"production"};
    
    // Request metadata
    std::string httpMethod;
    std::string urlPath;
    std::unordered_map<std::string, std::string> queryParameters;
    std::unordered_map<std::string, std::string> requestHeaders;
    std::string requestBody;

    // Recorded response
    int responseStatus{200};
    std::unordered_map<std::string, std::string> responseHeaders;
    std::string responseBody;
    double latencyMs{45.0};
    std::string recordedAt;
    
    bool isSanitized{true};
    std::vector<std::string> redactedKeys;
};

class DataRedactor {
public:
    // Redacts sensitive headers and sensitive JSON keys (passwords, tokens, cards, etc.)
    static void redactRecording(RequestRecording& recording);
    
    // Checks if key is deemed sensitive by default
    static bool isSensitiveKey(const std::string& key);
};

} // namespace traceweaver::replay
