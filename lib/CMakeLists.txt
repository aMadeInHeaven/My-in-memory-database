file(GLOB_RECURSE LIB_SOURCES 
    "BaseCommands/*.cpp"
    "Engine/*.cpp"
    "CommandCore/*.cpp"
)

add_library(vdb_core STATIC ${LIB_SOURCES})

target_include_directories(vdb_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
