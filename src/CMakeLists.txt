add_executable(labwork10 main.cpp)

set_target_properties(labwork10 PROPERTIES OUTPUT_NAME "labwork10")

target_link_libraries(labwork10 vdb_core)

set_target_properties(labwork10 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
