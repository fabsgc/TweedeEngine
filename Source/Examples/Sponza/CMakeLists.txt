# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        Sponza WIN32
        ${TE_SPONZA_SRC}
        ../resources.rc
    )
    set_target_properties(Sponza PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x64/Debug")
else ()
    add_executable(
        Sponza
        ${TE_SPONZA_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (Sponza PRIVATE -DTE_ENGINE_BUILD)

# Libraries
## Local libs
target_link_libraries (Sponza tef)
