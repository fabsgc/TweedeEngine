# Source files and their filters
include(CMakeSources.cmake)

message("-- TeGLRenderAPI Initialization...")

# Find packages
if(LINUX)
    find_package(OpenGL REQUIRED)
    find_package(X11)
endif()
    
# Target
add_library (TeGLRenderAPI SHARED ${TE_GLRENDERAPI_SRC})

# Defines
target_compile_definitions (TeGLRenderAPI PRIVATE 
    -DTE_ENGINE_BUILD
    -DGLEW_STATIC
    -DTE_CONFIG_DEBUG=1
    -DTE_CONFIG_RELWITHDEBINFO=2
    -DTE_CONFIG_MINSIZEREL=3
    -DTE_CONFIG_RELEASE=4
    $<$<CONFIG:Debug>:TE_CONFIG=1>
    $<$<CONFIG:RelWithDebInfo>:TE_CONFIG=2>
    $<$<CONFIG:MinSizeRel>:TE_CONFIG=3>
    $<$<CONFIG:Release>:TE_CONFIG=4>)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.3) # Windows 8.1
        target_compile_definitions (TeGLRenderAPI PRIVATE -DTE_WIN_SDK_8_1)
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
endif()

## Local libs
target_link_libraries (TeGLRenderAPI PUBLIC tef)

# IDE specific
set_property (TARGET TeGLRenderAPI PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeGLRenderAPI)
endif()

# Install
install_tef_target (TeGLRenderAPI)

message("-- ...TeGLRenderAPI OK.")