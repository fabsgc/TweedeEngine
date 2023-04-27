# Source files and their filters
include(CMakeSources.cmake)

if (WIN32)
    add_executable(
        Template WIN32
        ${TE_TEMPLATE_SRC}
        ../resources.rc
    )
    set_target_properties(Template PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_COMPILER}.Debug.${PLATFORM_TARGET}")
else ()
    add_executable(
        Template
        ${TE_TEMPLATE_SRC}
    )
endif ()

# If it's a standalone project (without engine build), you should comment this line
target_compile_definitions (Template PRIVATE 
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
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.3) # Windows 8.1
        target_compile_definitions (TeOpenAudio PRIVATE -DTE_WIN_SDK_8_1)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeShaderImporter PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Libraries
## Local libs
target_link_libraries (Template tef) 
