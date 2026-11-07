#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace traceweaver::testing {

enum class TestType {
    UNIT,
    INTEGRATION,
    CONTRACT,
    REGRESSION,
    END_TO_END
};

[[nodiscard]] constexpr std::string_view testTypeToString(TestType type) noexcept {
    switch (type) {
        case TestType::UNIT:        return "UNIT";
        case TestType::INTEGRATION: return "INTEGRATION";
        case TestType::CONTRACT:    return "CONTRACT";
        case TestType::REGRESSION:  return "REGRESSION";
        case TestType::END_TO_END:  return "END_TO_END";
    }
    return "UNIT";
}

struct TestCase {
    std::string testId;
    std::string name;
    std::string description;
    std::string associatedServiceId;
    std::string associatedOperationId;
    std::vector<std::string> relatedSchemaFields;
    TestType type{TestType::REGRESSION};
    int priority{1}; // 1 = Highest, 5 = Lowest
    double executionDurationMs{150.0};
    std::string lastExecutionResult{"PASSED"};
};

} // namespace traceweaver::testing
