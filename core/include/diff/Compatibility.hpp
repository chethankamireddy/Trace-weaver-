#pragma once

#include <string_view>

namespace traceweaver::diff {

enum class Compatibility {
    BREAKING,
    POTENTIALLY_BREAKING,
    NON_BREAKING,
    UNKNOWN
};

[[nodiscard]] constexpr std::string_view compatibilityToString(Compatibility comp) noexcept {
    switch (comp) {
        case Compatibility::BREAKING:             return "BREAKING";
        case Compatibility::POTENTIALLY_BREAKING: return "POTENTIALLY_BREAKING";
        case Compatibility::NON_BREAKING:         return "NON_BREAKING";
        case Compatibility::UNKNOWN:              return "UNKNOWN";
    }
    return "UNKNOWN";
}

enum class ChangeSeverity {
    INFO,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

[[nodiscard]] constexpr std::string_view severityToString(ChangeSeverity sev) noexcept {
    switch (sev) {
        case ChangeSeverity::INFO:     return "INFO";
        case ChangeSeverity::LOW:      return "LOW";
        case ChangeSeverity::MEDIUM:   return "MEDIUM";
        case ChangeSeverity::HIGH:     return "HIGH";
        case ChangeSeverity::CRITICAL: return "CRITICAL";
    }
    return "INFO";
}

enum class ChangeCategory {
    ENDPOINT,
    REQUEST_PARAMETER,
    REQUEST_BODY,
    RESPONSE_STATUS,
    RESPONSE_FIELD,
    AUTHENTICATION
};

[[nodiscard]] constexpr std::string_view categoryToString(ChangeCategory cat) noexcept {
    switch (cat) {
        case ChangeCategory::ENDPOINT:          return "ENDPOINT";
        case ChangeCategory::REQUEST_PARAMETER: return "REQUEST_PARAMETER";
        case ChangeCategory::REQUEST_BODY:      return "REQUEST_BODY";
        case ChangeCategory::RESPONSE_STATUS:   return "RESPONSE_STATUS";
        case ChangeCategory::RESPONSE_FIELD:    return "RESPONSE_FIELD";
        case ChangeCategory::AUTHENTICATION:    return "AUTHENTICATION";
    }
    return "ENDPOINT";
}

} // namespace traceweaver::diff
