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

# Libraries
## Local libs
target_link_libraries (Template tef) 
