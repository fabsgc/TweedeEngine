# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        Editor WIN32
        ${TE_EDITOR_SRC}
        ../resources.rc
    )
    set_target_properties(Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_COMPILER}.Debug.${PLATFORM_TARGET}")
else ()
    add_executable(
        Editor
        ${TE_EDITOR_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (Editor PRIVATE -DTE_ENGINE_BUILD)

# Libraries
## Local libs
target_link_libraries (Editor tef) 
