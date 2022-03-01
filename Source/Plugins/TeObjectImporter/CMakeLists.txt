# Source files and their filters
include(CMakeSources.cmake)

message("-- TeObjectImporter Initialization...")

# Target
add_library (TeObjectImporter SHARED ${TE_OBJECTIMPORTER_SRC})

# Defines
target_compile_definitions (TeObjectImporter PRIVATE -DTE_OBJECT_EXPORTS -DTE_ENGINE_BUILD)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeObjectImporter PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeObjectImporter PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeObjectImporter PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeObjectImporter PRIVATE "./")

# Libraries
## External libs: FLAC, Vorbis, Ogg, OpenAL
target_link_libraries(TeObjectImporter ${assimp_LIBRARIES})

## Local libs
target_link_libraries (TeObjectImporter tef)

# IDE specific
set_property (TARGET TeObjectImporter PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeObjectImporter)
endif()

# Install
install_tef_target (TeObjectImporter)

message("-- ...TeObjectImporter OK.")
