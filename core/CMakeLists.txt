cmake_minimum_required(VERSION 3.20)

# Core library
add_library(traceweaver_core
    src/graph/Graph.cpp
    src/graph/Algorithms.cpp
)

target_include_directories(traceweaver_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(traceweaver_core PUBLIC cxx_std_20)

# Testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Option to build with GTest if network/archive is available, or use standalone test runner
option(BUILD_STANDALONE_TESTS "Build standalone test binary without external GTest fetch" OFF)

if (NOT BUILD_STANDALONE_TESTS)
    FetchContent_MakeAvailable(googletest)
    
    add_executable(traceweaver_core_tests
        tests/graph/GraphTests.cpp
    )
    target_link_libraries(traceweaver_core_tests PRIVATE
        traceweaver_core
        GTest::gtest_main
    )
    add_test(NAME core_graph_tests COMMAND traceweaver_core_tests)
endif()

# Also build a lightweight standalone test executable for fast offline compilation
add_executable(standalone_graph_tests
    tests/graph/StandaloneGraphTests.cpp
)
target_link_libraries(standalone_graph_tests PRIVATE
    traceweaver_core
)
add_test(NAME standalone_core_graph_tests COMMAND standalone_graph_tests)
