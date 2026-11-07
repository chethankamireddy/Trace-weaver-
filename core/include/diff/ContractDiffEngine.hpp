#pragma once

#include "parser/Specification.hpp"
#include "DiffReport.hpp"

namespace traceweaver::diff {

class ContractDiffEngine {
public:
    ContractDiffEngine() = default;

    // Compares two normalized API specifications and produces a structured change report
    [[nodiscard]] DiffReport compare(
        const parser::ApiSpecification& oldSpec,
        const parser::ApiSpecification& newSpec);

private:
    void compareOperations(
        const parser::ApiOperation& oldOp,
        const parser::ApiOperation& newOp,
        std::vector<ApiChange>& changes);

    void compareSchemas(
        const std::string& parentPath,
        const std::string& operationId,
        const parser::SchemaObject& oldSchema,
        const parser::SchemaObject& newSchema,
        bool isRequest,
        std::vector<ApiChange>& changes);
};

} // namespace traceweaver::diff
