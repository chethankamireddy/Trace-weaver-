#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

namespace traceweaver::parser {

enum class SchemaType {
    STRING,
    INTEGER,
    NUMBER,
    BOOLEAN,
    ARRAY,
    OBJECT,
    ANY
};

[[nodiscard]] constexpr const char* schemaTypeToString(SchemaType type) noexcept {
    switch (type) {
        case SchemaType::STRING:  return "string";
        case SchemaType::INTEGER: return "integer";
        case SchemaType::NUMBER:  return "number";
        case SchemaType::BOOLEAN: return "boolean";
        case SchemaType::ARRAY:   return "array";
        case SchemaType::OBJECT:  return "object";
        case SchemaType::ANY:     return "any";
    }
    return "any";
}

struct SchemaField {
    std::string name;
    SchemaType type{SchemaType::ANY};
    std::string format;
    bool required{false};
    bool nullable{false};
    std::string description;
    std::vector<std::string> enumValues;
    std::shared_ptr<struct SchemaObject> itemSchema; // For array items
    std::shared_ptr<struct SchemaObject> nestedSchema; // For nested objects
    std::string refPath;
};

struct SchemaObject {
    std::string name;
    SchemaType type{SchemaType::OBJECT};
    std::string description;
    bool nullable{false};
    std::unordered_map<std::string, SchemaField> properties;
    std::vector<std::string> requiredFields;
    std::shared_ptr<SchemaObject> items; // For top-level array schemas
    std::vector<std::string> enumValues;
    std::string refPath;
    bool isCircularReference{false};
};

} // namespace traceweaver::parser
