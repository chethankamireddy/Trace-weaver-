#pragma once

#include "Compatibility.hpp"
#include <string>
#include <string_view>

namespace traceweaver::diff {

enum class ChangeType {
    ENDPOINT_REMOVED,
    ENDPOINT_ADDED,
    HTTP_METHOD_CHANGED,
    PATH_CHANGED,
    OPERATION_ID_CHANGED,
    
    REQUIRED_PARAMETER_ADDED,
    PARAMETER_REMOVED,
    PARAMETER_TYPE_CHANGED,
    PARAMETER_CONSTRAINT_TIGHTENED,
    
    REQUEST_BODY_SCHEMA_CHANGED,
    REQUIRED_REQUEST_FIELD_ADDED,
    OPTIONAL_REQUEST_FIELD_ADDED,
    REQUEST_FIELD_REMOVED,
    
    RESPONSE_FIELD_REMOVED,
    RESPONSE_FIELD_RENAMED,
    RESPONSE_FIELD_TYPE_CHANGED,
    REQUIRED_RESPONSE_FIELD_INTRODUCED,
    OPTIONAL_RESPONSE_FIELD_ADDED,
    ENUM_VALUES_REMOVED,
    RESPONSE_STATUS_CODE_REMOVED,
    RESPONSE_SCHEMA_CHANGED
};

[[nodiscard]] constexpr std::string_view changeTypeToString(ChangeType type) noexcept {
    switch (type) {
        case ChangeType::ENDPOINT_REMOVED:                  return "ENDPOINT_REMOVED";
        case ChangeType::ENDPOINT_ADDED:                    return "ENDPOINT_ADDED";
        case ChangeType::HTTP_METHOD_CHANGED:               return "HTTP_METHOD_CHANGED";
        case ChangeType::PATH_CHANGED:                      return "PATH_CHANGED";
        case ChangeType::OPERATION_ID_CHANGED:              return "OPERATION_ID_CHANGED";
        case ChangeType::REQUIRED_PARAMETER_ADDED:          return "REQUIRED_PARAMETER_ADDED";
        case ChangeType::PARAMETER_REMOVED:                 return "PARAMETER_REMOVED";
        case ChangeType::PARAMETER_TYPE_CHANGED:            return "PARAMETER_TYPE_CHANGED";
        case ChangeType::PARAMETER_CONSTRAINT_TIGHTENED:    return "PARAMETER_CONSTRAINT_TIGHTENED";
        case ChangeType::REQUEST_BODY_SCHEMA_CHANGED:       return "REQUEST_BODY_SCHEMA_CHANGED";
        case ChangeType::REQUIRED_REQUEST_FIELD_ADDED:      return "REQUIRED_REQUEST_FIELD_ADDED";
        case ChangeType::OPTIONAL_REQUEST_FIELD_ADDED:      return "OPTIONAL_REQUEST_FIELD_ADDED";
        case ChangeType::REQUEST_FIELD_REMOVED:             return "REQUEST_FIELD_REMOVED";
        case ChangeType::RESPONSE_FIELD_REMOVED:            return "RESPONSE_FIELD_REMOVED";
        case ChangeType::RESPONSE_FIELD_RENAMED:            return "RESPONSE_FIELD_RENAMED";
        case ChangeType::RESPONSE_FIELD_TYPE_CHANGED:       return "RESPONSE_FIELD_TYPE_CHANGED";
        case ChangeType::REQUIRED_RESPONSE_FIELD_INTRODUCED: return "REQUIRED_RESPONSE_FIELD_INTRODUCED";
        case ChangeType::OPTIONAL_RESPONSE_FIELD_ADDED:     return "OPTIONAL_RESPONSE_FIELD_ADDED";
        case ChangeType::ENUM_VALUES_REMOVED:               return "ENUM_VALUES_REMOVED";
        case ChangeType::RESPONSE_STATUS_CODE_REMOVED:      return "RESPONSE_STATUS_CODE_REMOVED";
        case ChangeType::RESPONSE_SCHEMA_CHANGED:           return "RESPONSE_SCHEMA_CHANGED";
    }
    return "UNKNOWN";
}

struct ApiChange {
    std::string changeId;
    ChangeType type;
    ChangeCategory category;
    ChangeSeverity severity;
    Compatibility compatibility;
    std::string oldValue;
    std::string newValue;
    std::string jsonPointer;
    std::string operationId;
    std::string description;
    std::string reason;
};

} // namespace traceweaver::diff
