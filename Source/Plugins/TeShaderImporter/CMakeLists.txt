# Source files and their filters
include(CMakeSources.cmake)
    
# Target
add_library (TeShaderImporter SHARED ${TE_SHADERIMPORTER_SRC})

# Defines
target_compile_definitions (TeShaderImporter PRIVATE -DTE_SHADER_EXPORTS)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeShaderImporter PRIVATE "./")

# Libraries
## Local libs
target_link_libraries (TeShaderImporter tef)

# IDE specific
set_property (TARGET TeShaderImporter PROPERTY FOLDER Plugins)

# Install
if (!WIN32)
    install_tef_target (TeShaderImporter)
endif ()