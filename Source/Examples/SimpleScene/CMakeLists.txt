# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        SimpleScene WIN32
        ${TE_SIMPLESCENE_SRC}
    )
    set_target_properties(SimpleScene PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x64/Debug")
else ()
    add_executable(
        SimpleScene
        ${TE_SIMPLESCENE_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (SimpleScene PRIVATE -DTE_ENGINE_BUILD)

# Libraries
## Local libs
target_link_libraries (SimpleScene tef) 
