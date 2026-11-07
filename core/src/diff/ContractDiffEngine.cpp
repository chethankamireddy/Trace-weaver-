#include "diff/ContractDiffEngine.hpp"
#include <algorithm>

namespace traceweaver::diff {

DiffReport ContractDiffEngine::compare(
    const parser::ApiSpecification& oldSpec,
    const parser::ApiSpecification& newSpec) {

    DiffReport report;
    report.oldVersion = oldSpec.version;
    report.newVersion = newSpec.version;

    // 1. Check for removed endpoints or changed operations
    for (const auto& [opKey, oldOp] : oldSpec.operations) {
        auto it = newSpec.operations.find(opKey);
        if (it == newSpec.operations.end()) {
            report.changes.push_back(ApiChange{
                .changeId = "chg_ep_rem_" + oldOp.operationId,
                .type = ChangeType::ENDPOINT_REMOVED,
                .category = ChangeCategory::ENDPOINT,
                .severity = ChangeSeverity::CRITICAL,
                .compatibility = Compatibility::BREAKING,
                .oldValue = oldOp.method + " " + oldOp.path,
                .newValue = "",
                .jsonPointer = "/paths/" + oldOp.path + "/" + oldOp.method,
                .operationId = oldOp.operationId,
                .description = "API endpoint " + oldOp.method + " " + oldOp.path + " was completely removed",
                .reason = "Consumers calling this endpoint will receive HTTP 404 Not Found"
            });
        } else {
            compareOperations(oldOp, it->second, report.changes);
        }
    }

    // 2. Check for newly introduced endpoints
    for (const auto& [opKey, newOp] : newSpec.operations) {
        if (!oldSpec.operations.contains(opKey)) {
            report.changes.push_back(ApiChange{
                .changeId = "chg_ep_add_" + newOp.operationId,
                .type = ChangeType::ENDPOINT_ADDED,
                .category = ChangeCategory::ENDPOINT,
                .severity = ChangeSeverity::INFO,
                .compatibility = Compatibility::NON_BREAKING,
                .oldValue = "",
                .newValue = newOp.method + " " + newOp.path,
                .jsonPointer = "/paths/" + newOp.path + "/" + newOp.method,
                .operationId = newOp.operationId,
                .description = "New API endpoint " + newOp.method + " " + newOp.path + " was introduced",
                .reason = "Introducing a new endpoint does not break existing clients"
            });
        }
    }

    // 3. Compute statistics
    report.totalChanges = report.changes.size();
    for (const auto& chg : report.changes) {
        if (chg.compatibility == Compatibility::BREAKING) {
            ++report.breakingCount;
        } else if (chg.compatibility == Compatibility::POTENTIALLY_BREAKING) {
            ++report.potentiallyBreakingCount;
        } else if (chg.compatibility == Compatibility::NON_BREAKING) {
            ++report.nonBreakingCount;
        }
    }

    return report;
}

void ContractDiffEngine::compareOperations(
    const parser::ApiOperation& oldOp,
    const parser::ApiOperation& newOp,
    std::vector<ApiChange>& changes) {

    // Check request parameters
    std::unordered_map<std::string, parser::Parameter> oldParams;
    for (const auto& p : oldOp.parameters) {
        oldParams[p.name] = p;
    }

    std::unordered_map<std::string, parser::Parameter> newParams;
    for (const auto& p : newOp.parameters) {
        newParams[p.name] = p;
    }

    // Added parameters
    for (const auto& [pName, newParam] : newParams) {
        if (!oldParams.contains(pName)) {
            if (newParam.required) {
                changes.push_back(ApiChange{
                    .changeId = "chg_param_req_" + oldOp.operationId + "_" + pName,
                    .type = ChangeType::REQUIRED_PARAMETER_ADDED,
                    .category = ChangeCategory::REQUEST_PARAMETER,
                    .severity = ChangeSeverity::HIGH,
                    .compatibility = Compatibility::BREAKING,
                    .oldValue = "",
                    .newValue = pName + " (required)",
                    .jsonPointer = "/paths/" + oldOp.path + "/parameters/" + pName,
                    .operationId = oldOp.operationId,
                    .description = "New required parameter '" + pName + "' added to " + oldOp.operationId,
                    .reason = "Existing callers without this parameter will fail request validation"
                });
            } else {
                changes.push_back(ApiChange{
                    .changeId = "chg_param_opt_" + oldOp.operationId + "_" + pName,
                    .type = ChangeType::OPTIONAL_REQUEST_FIELD_ADDED,
                    .category = ChangeCategory::REQUEST_PARAMETER,
                    .severity = ChangeSeverity::LOW,
                    .compatibility = Compatibility::NON_BREAKING,
                    .oldValue = "",
                    .newValue = pName + " (optional)",
                    .jsonPointer = "/paths/" + oldOp.path + "/parameters/" + pName,
                    .operationId = oldOp.operationId,
                    .description = "New optional parameter '" + pName + "' added to " + oldOp.operationId,
                    .reason = "Optional parameters do not impact existing callers"
                });
            }
        }
    }

    // Removed parameters
    for (const auto& [pName, oldParam] : oldParams) {
        if (!newParams.contains(pName)) {
            changes.push_back(ApiChange{
                .changeId = "chg_param_rem_" + oldOp.operationId + "_" + pName,
                .type = ChangeType::PARAMETER_REMOVED,
                .category = ChangeCategory::REQUEST_PARAMETER,
                .severity = ChangeSeverity::MEDIUM,
                .compatibility = Compatibility::POTENTIALLY_BREAKING,
                .oldValue = pName,
                .newValue = "",
                .jsonPointer = "/paths/" + oldOp.path + "/parameters/" + pName,
                .operationId = oldOp.operationId,
                .description = "Parameter '" + pName + "' was removed from " + oldOp.operationId,
                .reason = "Clients relying on this parameter may experience degraded behavior"
            });
        }
    }

    // Check responses (200, 201, etc.)
    for (const auto& [statusCode, oldResp] : oldOp.responses) {
        auto it = newOp.responses.find(statusCode);
        if (it == newOp.responses.end()) {
            changes.push_back(ApiChange{
                .changeId = "chg_resp_status_rem_" + oldOp.operationId + "_" + std::to_string(statusCode),
                .type = ChangeType::RESPONSE_STATUS_CODE_REMOVED,
                .category = ChangeCategory::RESPONSE_STATUS,
                .severity = ChangeSeverity::HIGH,
                .compatibility = Compatibility::BREAKING,
                .oldValue = std::to_string(statusCode),
                .newValue = "",
                .jsonPointer = "/paths/" + oldOp.path + "/responses/" + std::to_string(statusCode),
                .operationId = oldOp.operationId,
                .description = "Response status code " + std::to_string(statusCode) + " was removed from " + oldOp.operationId,
                .reason = "Consumers expecting this HTTP status code may encounter unhandled state"
            });
        } else if (oldResp.schema && it->second.schema) {
            compareSchemas(
                "/paths/" + oldOp.path + "/responses/" + std::to_string(statusCode) + "/content/schema",
                oldOp.operationId,
                *oldResp.schema,
                *it->second.schema,
                false,
                changes);
        }
    }
}

void ContractDiffEngine::compareSchemas(
    const std::string& parentPath,
    const std::string& operationId,
    const parser::SchemaObject& oldSchema,
    const parser::SchemaObject& newSchema,
    bool isRequest,
    std::vector<ApiChange>& changes) {

    // Removed response properties
    for (const auto& [propName, oldField] : oldSchema.properties) {
        auto it = newSchema.properties.find(propName);
        if (it == newSchema.properties.end()) {
            if (!isRequest) {
                changes.push_back(ApiChange{
                    .changeId = "chg_field_rem_" + operationId + "_" + propName,
                    .type = ChangeType::RESPONSE_FIELD_REMOVED,
                    .category = ChangeCategory::RESPONSE_FIELD,
                    .severity = ChangeSeverity::CRITICAL,
                    .compatibility = Compatibility::BREAKING,
                    .oldValue = propName + " (" + schemaTypeToString(oldField.type) + ")",
                    .newValue = "",
                    .jsonPointer = parentPath + "/properties/" + propName,
                    .operationId = operationId,
                    .description = "Response field '" + propName + "' was removed",
                    .reason = "Downstream consumers and clients reading this field will encounter null/undefined crashes"
                });
            }
        } else {
            // Type changes
            if (oldField.type != it->second.type) {
                changes.push_back(ApiChange{
                    .changeId = "chg_field_type_" + operationId + "_" + propName,
                    .type = ChangeType::RESPONSE_FIELD_TYPE_CHANGED,
                    .category = ChangeCategory::RESPONSE_FIELD,
                    .severity = ChangeSeverity::HIGH,
                    .compatibility = Compatibility::BREAKING,
                    .oldValue = schemaTypeToString(oldField.type),
                    .newValue = schemaTypeToString(it->second.type),
                    .jsonPointer = parentPath + "/properties/" + propName,
                    .operationId = operationId,
                    .description = "Field '" + propName + "' type changed from " + schemaTypeToString(oldField.type) + " to " + schemaTypeToString(it->second.type),
                    .reason = "Type deserialization in downstream microservices will fail"
                });
            }
        }
    }

    // Added response properties
    for (const auto& [propName, newField] : newSchema.properties) {
        if (!oldSchema.properties.contains(propName)) {
            if (!isRequest) {
                changes.push_back(ApiChange{
                    .changeId = "chg_field_add_" + operationId + "_" + propName,
                    .type = ChangeType::OPTIONAL_RESPONSE_FIELD_ADDED,
                    .category = ChangeCategory::RESPONSE_FIELD,
                    .severity = ChangeSeverity::LOW,
                    .compatibility = Compatibility::NON_BREAKING,
                    .oldValue = "",
                    .newValue = propName + " (" + schemaTypeToString(newField.type) + ")",
                    .jsonPointer = parentPath + "/properties/" + propName,
                    .operationId = operationId,
                    .description = "New response field '" + propName + "' added",
                    .reason = "Adding fields to responses is backward-compatible for tolerant readers"
                });
            }
        }
    }
}

} // namespace traceweaver::diff
