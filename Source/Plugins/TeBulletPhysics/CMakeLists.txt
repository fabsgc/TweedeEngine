# Source files and their filters
include(CMakeSources.cmake)

# Find packages
if(PHYSICS_MODULE MATCHES "BulletPhysics")

endif()

# Target
add_library (TeBulletPhysics SHARED ${TE_BULLETPHYSICS_SRC})

# Defines
target_compile_definitions (TeBulletPhysics PRIVATE -DTE_BULLETPHYSICS_EXPORTS -DTE_ENGINE_BUILD)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeBulletPhysics PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeBulletPhysics PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeBulletPhysics PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeBulletPhysics PRIVATE "./")

# Libraries

## Local libs
target_link_libraries (TeBulletPhysics tef)

# IDE specific
set_property (TARGET TeBulletPhysics PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeBulletPhysics)
endif()

# Install
if(PHYSICS_MODULE MATCHES "BulletPhysics")
    install_tef_target(TeBulletPhysics)
endif()
