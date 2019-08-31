# Version
set (TE_FRAMEWORK_VERSION_MAJOR 0)
set (TE_FRAMEWORK_VERSION_MINOR 1)

# Options
if (WIN32)
    set (RENDER_API_MODULE "DirectX 11" CACHE STRING "Render API to use.")
    set_property (CACHE RENDER_API_MODULE PROPERTY STRINGS "DirectX 11" "OpenGL")
else ()
    set (RENDER_API_MODULE "OpenGL" CACHE STRING "Render API to use.")
    set_property (CACHE RENDER_API_MODULE PROPERTY STRINGS "OpenGL")
endif ()

set (RENDERER_MODULE "RenderMan" CACHE STRING "Renderer backend to use.")
set_property (CACHE RENDERER_MODULE PROPERTY STRINGS Renderer)

## Check dependencies built from source
if (WIN32)
    set(SOURCE_DEP_BUILD_DIR ${BSF_SOURCE_DIR}/../Dependencies/Build)
    set(SOURCE_DEP_BUILD_VERSION_FILE ${SOURCE_DEP_BUILD_DIR}/.version)
endif ()

# Config file 
## Set names of libraries used in the config file
if (RENDER_API_MODULE MATCHES "DirectX 11")
    set (RENDER_API_MODULE_LIB TeD3D11RenderAPI)
else ()
    set (RENDER_API_MODULE_LIB TeGLRenderAPI)
endif ()

set(RENDERER_MODULE_LIB TeRenderMan)

set(INCLUDE_ALL_IN_WORKFLOW true CACHE BOOL "If true, all libraries (even those not selected) will be included in the generated workflow (e.g. Visual Studio solution). This is useful when working on engine internals with a need for easy access to all parts of it. Only relevant for workflow generators like Visual Studio or XCode.")

## Generate config files)
configure_file("${TE_SOURCE_DIR}/CMake/TeEngineConfig.h.in" "${TE_SOURCE_DIR}/Framework/Core/TeEngineConfig.h")

## External libs
include (${CMAKE_MODULE_PATH}/Findassimp.cmake)
include (${CMAKE_MODULE_PATH}/Findfreeimg.cmake)

# Sub-directories
## Framework
add_subdirectory (Framework)

## Plugins
if (INCLUDE_ALL_IN_WORKFLOW AND MSVC)
    add_subdirectory (Plugins/TeD3D11RenderAPI)
    add_subdirectory (Plugins/TeGLRenderAPI)
else () # Otherwise include only chosen ones
    if (RENDER_API_MODULE MATCHES "DirectX 11")
        add_subdirectory (Plugins/TeD3D11RenderAPI)
    else ()
        add_subdirectory (Plugins/TeGLRenderAPI)
    endif ()
endif ()

add_subdirectory (Plugins/TeRenderMan)
add_subdirectory (Plugins/TeObjectImporter)
add_subdirectory (Plugins/TeFreeImgImporter)

add_subdirectory (Examples)

## Install
install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Data
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin/
)

## Copy data elements inside bin directory
if (TE_64BIT)
    set (PLATFORM_DATA "x64")
else ()
    set (PLATFORM_DATA "x86")
endif ()

execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Data" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/Release/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Data" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/RelWithDebInfo/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Data" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/MinSizeRel/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Data" "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/Debug/Data/")

execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/Release/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/RelWithDebInfo/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/MinSizeRel/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${PLATFORM_DATA}/Debug/Log/")