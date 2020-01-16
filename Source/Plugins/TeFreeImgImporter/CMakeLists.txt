# Source files and their filters
include(CMakeSources.cmake)
    
# Target
add_library (TeFreeImgImporter SHARED ${TE_FREEIMGIMPORTER_SRC})

# Defines
target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_FREEIMG_EXPORTS)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeFreeImgImporter PRIVATE "./")

# Libraries
## External libs: FLAC, Vorbis, Ogg, OpenAL
target_link_libraries(TeFreeImgImporter ${freeimg_LIBRARIES})

## Local libs
target_link_libraries (TeFreeImgImporter tef)

# IDE specific
set_property (TARGET TeFreeImgImporter PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeFreeImgImporter)
endif()

# Install
install_tef_target (TeFreeImgImporter)