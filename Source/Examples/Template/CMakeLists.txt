# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        Template WIN32
        ${TE_TEMPLATE_SRC}
    )
    set_target_properties(Template PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x64/Debug")
else ()
    add_executable(
        Template
        ${TE_TEMPLATE_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (Template PRIVATE -DTE_ENGINE_BUILD)

# Libraries
## Local libs
target_link_libraries (Template tef) 
