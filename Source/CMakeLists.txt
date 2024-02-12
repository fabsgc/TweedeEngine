# Version
set (TE_FRAMEWORK_VERSION_MAJOR 0)
set (TE_FRAMEWORK_VERSION_MINOR 1)

# Options
set(AUDIO_MODULE "OpenAudio" CACHE STRING "Audio backend to use.")
set_property(CACHE AUDIO_MODULE PROPERTY STRINGS OpenAudio)

set(PHYSICS_MODULE "BulletPhysics" CACHE STRING "Physics backend to use.")
set_property(CACHE PHYSICS_MODULE PROPERTY STRINGS "BulletPhysics")

if (WIN32)
    set (RENDER_API_MODULE "DirectX 11" CACHE STRING "Render API to use.")
    set_property (CACHE RENDER_API_MODULE PROPERTY STRINGS "DirectX 11" "OpenGL")
    set (GUI_API_MODULE "D3D11 ImGui" CACHE STRING "Render API to use.")
else ()
    set (RENDER_API_MODULE "OpenGL" CACHE STRING "Render API to use.")
    set_property (CACHE RENDER_API_MODULE PROPERTY STRINGS "OpenGL")
    set (GUI_API_MODULE "OpenGL ImGui" CACHE STRING "Render API to use.")
endif ()

set (RENDERER_MODULE "RenderMan" CACHE STRING "Renderer backend to use.")
set_property (CACHE RENDERER_MODULE PROPERTY STRINGS Renderer)

# Config file 
## Set names of libraries used in the config file
if (RENDER_API_MODULE MATCHES "DirectX 11")
    set (RENDER_API_MODULE_LIB TeD3D11RenderAPI)
    set (GUI_API_MODULE_LIB TeD3D11ImGuiAPI)
else ()
    set (RENDER_API_MODULE_LIB TeGLRenderAPI)
    set (GUI_API_MODULE_LIB TeGLImGuiAPI)
endif ()

set(AUDIO_MODULE_LIB TeOpenAudio)
set(RENDERER_MODULE_LIB TeRenderMan)
set(PHYSICS_MODULE_LIB TeBulletPhysics)

set(INCLUDE_ALL_IN_WORKFLOW true CACHE BOOL "If true, all libraries (even those not selected) will be included in the generated workflow (e.g. Visual Studio solution). This is useful when working on engine internals with a need for easy access to all parts of it. Only relevant for workflow generators like Visual Studio or XCode.")

## Generate config files
configure_file("${TE_SOURCE_DIR}/CMake/TeEngineConfig.h.in" "${TE_SOURCE_DIR}/Framework/Core/TeEngineConfig.h")

set(USE_BUNDLED_LIBRARIES ON CACHE BOOL "Use and install bundled libraries")

## External libs
include (${CMAKE_MODULE_PATH}/Findassimp.cmake)
include (${CMAKE_MODULE_PATH}/Findfreeimg.cmake)
include (${CMAKE_MODULE_PATH}/Findnvtt.cmake)
include (${CMAKE_MODULE_PATH}/Findfreetype.cmake)
include (${CMAKE_MODULE_PATH}/Findbullet.cmake)

## Remove warning
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Sub-directories
## Framework
add_subdirectory (Framework)

## Plugins
if (INCLUDE_ALL_IN_WORKFLOW AND MSVC)
    add_subdirectory (Plugins/TeD3D11RenderAPI)
    add_subdirectory (Plugins/TeGLRenderAPI)
    add_subdirectory (Plugins/TeD3D11ImGuiAPI)
    add_subdirectory (Plugins/TeGLImGuiAPI)
else () # Otherwise include only chosen ones
    if (RENDER_API_MODULE MATCHES "DirectX 11")
        add_subdirectory (Plugins/TeD3D11RenderAPI)
        add_subdirectory (Plugins/TeD3D11ImGuiAPI)
    else ()
        add_subdirectory (Plugins/TeGLRenderAPI)
        add_subdirectory (Plugins/TeGLImGuiAPI)
    endif ()
endif ()

add_subdirectory (Plugins/TeRenderMan)
add_subdirectory (Plugins/TeObjectImporter)
add_subdirectory (Plugins/TeFreeImgImporter)
add_subdirectory (Plugins/TeFontImporter)
add_subdirectory (Plugins/TeShaderImporter)
add_subdirectory (Plugins/TeOpenAudio)
add_subdirectory (Plugins/TeBulletPhysics)
add_subdirectory (Plugins/TeSceneImporter)
add_subdirectory (Plugins/TeSceneExporter)

add_subdirectory (Examples)

## Install
install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Data
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin/
)

execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_ROOT_DIR}/Data" "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.Release.${PLATFORM_TARGET}/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_ROOT_DIR}/Data" "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.RelWithDebInfo.${PLATFORM_TARGET}/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_ROOT_DIR}/Data" "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.MinSizeRel.${PLATFORM_TARGET}/Data/")
execute_process (COMMAND ${CMAKE_COMMAND} -E copy_directory "${APP_ROOT_DIR}/Data" "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.Debug.${PLATFORM_TARGET}/Data/")

execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.Release.${PLATFORM_TARGET}/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.RelWithDebInfo.${PLATFORM_TARGET}/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.MinSizeRel.${PLATFORM_TARGET}/Log/")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_ROOT_DIR}/bin/${PLATFORM_COMPILER}.Debug.${PLATFORM_TARGET}/Log/")

if (WIN32)
    add_custom_target(AlwaysBuild ALL DEPENDS "${APP_ROOT_DIR}/Source/CMake/ForceBuild.cpp")
    install_pre_build_data(AlwaysBuild)
endif()
