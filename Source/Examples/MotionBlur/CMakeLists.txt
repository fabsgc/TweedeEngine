# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

if (WIN32)
    add_executable(
        MotionBlur WIN32
        ${TE_MOTIONBLUR_SRC}
    )
    set_target_properties(MotionBlur PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/x64/Debug")
else ()
    add_executable(
        MotionBlur
        ${TE_MOTIONBLUR_SRC}
    )
endif ()

# Libraries
## Local libs
target_link_libraries (MotionBlur tef) 
