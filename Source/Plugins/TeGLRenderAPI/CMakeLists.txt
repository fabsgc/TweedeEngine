# Source files and their filters
include(CMakeSources.cmake)

# Find packages
if(LINUX)
    find_package(OpenGL REQUIRED)
    find_package(X11)
endif()
    
# Target
add_library (TeGLRenderAPI SHARED ${TE_GLRENDERAPI_SRC})

# Defines
target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_GL_EXPORTS)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeGLRenderAPI PRIVATE "./")

# Libraries
if(WIN32)
    ## External lib: OpenGL
    target_link_libraries (TeGLRenderAPI PRIVATE glu32 opengl32)
elseif(LINUX)
    ## External lib: OpenGL
    target_link_libraries (TeGLRenderAPI PRIVATE ${OPENGL_LIBRARIES})
elseif(APPLE) # MacOS
    target_link_framework (TeGLRenderAPI Foundation)
    target_link_framework (TeGLRenderAPI CoreGraphics)
    target_link_framework (TeGLRenderAPI CoreVideo)
    target_link_framework (TeGLRenderAPI OpenGL)
    target_link_framework (TeGLRenderAPI IOKit)
    target_link_framework (TeGLRenderAPI AppKit)
endif()

## Local libs
target_link_libraries (TeGLRenderAPI PUBLIC tef)

# IDE specific
set_property (TARGET TeGLRenderAPI PROPERTY FOLDER Plugins)

# Install
if (RENDER_API_MODULE MATCHES "OpenGL" AND !WIN32)
    install_tef_target (TeGLRenderAPI)
endif()