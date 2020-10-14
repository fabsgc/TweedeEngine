# Source files and their filters
include(CMakeSources.cmake)

# Find packages
if(LINUX)
    find_package(OpenGL REQUIRED)
    find_package(X11)
endif()
    
# Target
add_library (TeGLImGuiAPI SHARED ${TE_GLIMGUIAPI_SRC})

# Defines
target_compile_definitions (TeGLImGuiAPI PRIVATE -DTE_GL_EXPORTS -DGLEW_STATIC)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeGLImGuiAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeGLImGuiAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeGLImGuiAPI PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeGLImGuiAPI PRIVATE "./")

# Libraries
if(WIN32)
    ## External lib: OpenGL
    target_link_libraries (TeGLImGuiAPI PRIVATE glu32 opengl32)
elseif(LINUX)
    ## External lib: OpenGL
    target_link_libraries (TeGLImGuiAPI PRIVATE ${OPENGL_LIBRARIES})
endif()

## Local libs
target_link_libraries (TeGLImGuiAPI PUBLIC tef)

# IDE specific
set_property (TARGET TeGLImGuiAPI PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeGLImGuiAPI)
endif()

# Install
install_tef_target (TeGLImGuiAPI)