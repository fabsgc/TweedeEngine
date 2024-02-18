# Source files and their filters
include(CMakeSources.cmake)

message("-- TeFreeImgImporter Initialization...")

# Target
add_library (TeFreeImgImporter SHARED ${TE_FREEIMGIMPORTER_SRC})

# Includes
target_include_directories (TeFreeImgImporter PRIVATE "./")

# Defines
target_compile_definitions (TeFreeImgImporter PRIVATE 
    -DTE_ENGINE_BUILD
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
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.3) # Windows 8.1
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_8_1)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeFreeImgImporter PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

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

message("-- ...TeFreeImgImporter OK.")
