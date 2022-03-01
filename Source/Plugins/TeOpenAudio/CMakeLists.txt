# Source files and their filters
include(CMakeSources.cmake)

message("-- TeOpenAudio Initialization...")

# Find packages
if(AUDIO_MODULE MATCHES "OpenAudio")
    find_package(OpenAL REQUIRED)
    find_package(ogg REQUIRED)
    find_package(vorbis REQUIRED)
    find_package(FLAC REQUIRED)
endif()

# Target
add_library (TeOpenAudio SHARED ${TE_OPENAUDIO_SRC})

# Defines
target_compile_definitions (TeOpenAudio PRIVATE -DTE_OBJECT_EXPORTS -DTE_ENGINE_BUILD)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeOpenAudio PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeOpenAudio PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeOpenAudio PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeOpenAudio PRIVATE "./")

# Libraries
## External libs: FLAC, Vorbis, Ogg, OpenAL
target_link_libraries(TeOpenAudio ${OpenAL_LIBRARIES})
target_link_libraries(TeOpenAudio ${vorbis_LIBRARIES})
target_link_libraries(TeOpenAudio ${FLAC_LIBRARIES})
target_link_libraries(TeOpenAudio ${ogg_LIBRARIES})

## Local libs
target_link_libraries (TeOpenAudio tef)

# IDE specific
set_property (TARGET TeOpenAudio PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeOpenAudio)
endif()

# Install
if(AUDIO_MODULE MATCHES "OpenAudio")
    install_tef_target(TeOpenAudio)
endif()

message("-- ...TeOpenAudio OK.")
