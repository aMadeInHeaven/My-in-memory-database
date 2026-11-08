cmake_minimum_required(VERSION 3.15)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    vdb_tests
    commands_tests_ut.cpp
    db_tests_ut.cpp
    engine_tests_ut.cpp
    memory_tests_ut.cpp
    parser_tests.cpp
    value_tests_ut.cpp
)

target_link_libraries(vdb_tests
    vdb_core
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(vdb_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/lib
)

set_target_properties(vdb_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(GoogleTest)
gtest_discover_tests(vdb_tests)
