# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        MotionBlur WIN32
        ${TE_MOTIONBLUR_SRC}
        ../resources.rc
    )
    set_target_properties(MotionBlur PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x64/Debug")
else ()
    add_executable(
        MotionBlur
        ${TE_MOTIONBLUR_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (MotionBlur PRIVATE -DTE_ENGINE_BUILD)

# Libraries
## Local libs
target_link_libraries (MotionBlur tef) 
