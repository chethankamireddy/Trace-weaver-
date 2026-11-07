#pragma once

#include "Operation.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace traceweaver::parser {

struct ApiSpecification {
    std::string serviceId;
    std::string title;
    std::string version;
    std::string openApiVersion;
    std::string description;
    
    // Normalized operations key: "METHOD:PATH" e.g. "GET:/api/v1/users/{id}"
    std::unordered_map<std::string, ApiOperation> operations;
    
    // Shared components schemas: schema name -> SchemaObject
    std::unordered_map<std::string, std::shared_ptr<SchemaObject>> components;

    [[nodiscard]] std::string makeOperationKey(const std::string& method, const std::string& path) const {
        return method + ":" + path;
    }
};

} // namespace traceweaver::parser
