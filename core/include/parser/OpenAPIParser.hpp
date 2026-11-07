#pragma once

#include "Specification.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <expected>
#include <memory>

namespace traceweaver::parser {

struct ParseError {
    std::string message;
    std::string jsonPointer;
    bool isFatal{true};
};

struct ParseResult {
    bool success{false};
    ApiSpecification specification;
    std::vector<ParseError> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> unsupportedFeatures;
};

class OpenAPIParser {
public:
    OpenAPIParser() = default;

    // Parses raw OpenAPI 3.0 or 3.1 JSON or YAML string
    [[nodiscard]] ParseResult parseString(std::string_view content, const std::string& serviceId = "");

    // Validates JSON / YAML schema syntax and structural conformity
    [[nodiscard]] static bool isValidSyntax(std::string_view content);

private:
    // Circular reference guard set containing current $ref traversal stack
    std::unordered_set<std::string> refStack_;
};

} // namespace traceweaver::parser
