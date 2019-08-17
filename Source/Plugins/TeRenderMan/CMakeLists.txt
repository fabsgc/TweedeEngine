# Source files and their filters
include(CMakeSources.cmake)
    
# Target
add_library (TeRenderMan SHARED ${TE_RENDERMAN_SRC})

# Defines
target_compile_definitions (TeRenderMan PRIVATE -DTE_RNDMAN_EXPORTS)

# Includes
target_include_directories (TeRenderMan PRIVATE "./")

# Libraries
## Local libs
target_link_libraries (TeRenderMan tef)

# IDE specific
set_property (TARGET TeRenderMan PROPERTY FOLDER Plugins)

# Install
if (!WIN32)
    install_tef_target (TeRenderMan)
endif ()