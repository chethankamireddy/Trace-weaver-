#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>

namespace traceweaver::graph {

enum class NodeType {
    SERVICE,
    API_OPERATION,
    SCHEMA_OBJECT,
    SCHEMA_FIELD,
    CLIENT,
    TEST_CASE
};

[[nodiscard]] constexpr std::string_view nodeTypeToString(NodeType type) noexcept {
    switch (type) {
        case NodeType::SERVICE:        return "SERVICE";
        case NodeType::API_OPERATION:  return "API_OPERATION";
        case NodeType::SCHEMA_OBJECT:  return "SCHEMA_OBJECT";
        case NodeType::SCHEMA_FIELD:   return "SCHEMA_FIELD";
        case NodeType::CLIENT:         return "CLIENT";
        case NodeType::TEST_CASE:      return "TEST_CASE";
    }
    return "UNKNOWN";
}

class Node {
public:
    Node(std::string id,
         NodeType type,
         std::string name,
         std::string description = "",
         std::unordered_map<std::string, std::string> metadata = {})
        : id_(std::move(id)),
          type_(type),
          name_(std::move(name)),
          description_(std::move(description)),
          metadata_(std::move(metadata)) {}

    [[nodiscard]] const std::string& getId() const noexcept { return id_; }
    [[nodiscard]] NodeType getType() const noexcept { return type_; }
    [[nodiscard]] std::string_view getTypeName() const noexcept { return nodeTypeToString(type_); }
    [[nodiscard]] const std::string& getName() const noexcept { return name_; }
    [[nodiscard]] const std::string& getDescription() const noexcept { return description_; }
    [[nodiscard]] const std::unordered_map<std::string, std::string>& getMetadata() const noexcept { return metadata_; }

    [[nodiscard]] std::string getMetadataValue(const std::string& key, const std::string& defaultValue = "") const {
        auto it = metadata_.find(key);
        return it != metadata_.end() ? it->second : defaultValue;
    }

    void setMetadata(std::string key, std::string value) {
        metadata_[std::move(key)] = std::move(value);
    }

    bool operator==(const Node& other) const noexcept {
        return id_ == other.id_;
    }

    bool operator!=(const Node& other) const noexcept {
        return !(*this == other);
    }

private:
    std::string id_;
    NodeType type_;
    std::string name_;
    std::string description_;
    std::unordered_map<std::string, std::string> metadata_;
};

} // namespace traceweaver::graph
