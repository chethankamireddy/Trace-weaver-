#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace traceweaver::graph {

enum class RelationshipType {
    SERVICE_DEPENDS_ON_SERVICE,
    OPERATION_USES_SCHEMA,
    SCHEMA_CONTAINS_FIELD,
    CLIENT_CALLS_OPERATION,
    TEST_COVERS_OPERATION,
    OPERATION_CALLS_OPERATION
};

[[nodiscard]] constexpr std::string_view relationshipTypeToString(RelationshipType rel) noexcept {
    switch (rel) {
        case RelationshipType::SERVICE_DEPENDS_ON_SERVICE: return "SERVICE_DEPENDS_ON_SERVICE";
        case RelationshipType::OPERATION_USES_SCHEMA:      return "OPERATION_USES_SCHEMA";
        case RelationshipType::SCHEMA_CONTAINS_FIELD:       return "SCHEMA_CONTAINS_FIELD";
        case RelationshipType::CLIENT_CALLS_OPERATION:     return "CLIENT_CALLS_OPERATION";
        case RelationshipType::TEST_COVERS_OPERATION:      return "TEST_COVERS_OPERATION";
        case RelationshipType::OPERATION_CALLS_OPERATION:  return "OPERATION_CALLS_OPERATION";
    }
    return "UNKNOWN";
}

class Edge {
public:
    Edge(std::string sourceId,
         std::string destinationId,
         RelationshipType relationship,
         double weight = 1.0,
         std::unordered_map<std::string, std::string> metadata = {})
        : sourceId_(std::move(sourceId)),
          destinationId_(std::move(destinationId)),
          relationship_(relationship),
          weight_(weight),
          metadata_(std::move(metadata)) {}

    [[nodiscard]] const std::string& getSourceId() const noexcept { return sourceId_; }
    [[nodiscard]] const std::string& getDestinationId() const noexcept { return destinationId_; }
    [[nodiscard]] RelationshipType getRelationship() const noexcept { return relationship_; }
    [[nodiscard]] std::string_view getRelationshipName() const noexcept { return relationshipTypeToString(relationship_); }
    [[nodiscard]] double getWeight() const noexcept { return weight_; }
    [[nodiscard]] const std::unordered_map<std::string, std::string>& getMetadata() const noexcept { return metadata_; }

    [[nodiscard]] std::string getMetadataValue(const std::string& key, const std::string& defaultValue = "") const {
        auto it = metadata_.find(key);
        return it != metadata_.end() ? it->second : defaultValue;
    }

    bool operator==(const Edge& other) const noexcept {
        return sourceId_ == other.sourceId_ &&
               destinationId_ == other.destinationId_ &&
               relationship_ == other.relationship_;
    }

    bool operator!=(const Edge& other) const noexcept {
        return !(*this == other);
    }

private:
    std::string sourceId_;
    std::string destinationId_;
    RelationshipType relationship_;
    double weight_;
    std::unordered_map<std::string, std::string> metadata_;
};

} // namespace traceweaver::graph
