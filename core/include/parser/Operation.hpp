#pragma once

#include "Schema.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace traceweaver::parser {

enum class ParameterIn {
    PATH,
    QUERY,
    HEADER,
    COOKIE
};

[[nodiscard]] constexpr const char* parameterInToString(ParameterIn in) noexcept {
    switch (in) {
        case ParameterIn::PATH:   return "path";
        case ParameterIn::QUERY:  return "query";
        case ParameterIn::HEADER: return "header";
        case ParameterIn::COOKIE: return "cookie";
    }
    return "query";
}

struct Parameter {
    std::string name;
    ParameterIn in{ParameterIn::QUERY};
    bool required{false};
    std::string description;
    SchemaType type{SchemaType::STRING};
    std::string format;
    std::vector<std::string> enumValues;
};

struct RequestBody {
    std::string description;
    bool required{false};
    std::string contentType{"application/json"};
    std::shared_ptr<SchemaObject> schema;
};

struct ResponseDefinition {
    int statusCode{200};
    std::string description;
    std::string contentType{"application/json"};
    std::shared_ptr<SchemaObject> schema;
};

struct ApiOperation {
    std::string operationId;
    std::string method; // GET, POST, PUT, DELETE, PATCH, etc.
    std::string path;
    std::string summary;
    std::string description;
    std::vector<Parameter> parameters;
    std::optional<RequestBody> requestBody;
    std::unordered_map<int, ResponseDefinition> responses; // Status code -> response
    std::vector<std::string> tags;
    std::vector<std::string> securityRequirements;
    std::vector<std::string> dependentSchemaRefs;
};

} // namespace traceweaver::parser
