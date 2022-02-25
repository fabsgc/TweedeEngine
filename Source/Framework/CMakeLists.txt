# Source files and their filters
include (Core/CMakeSources.cmake)
include (Utility/CMakeSources.cmake)

if (LINUX)
    find_package (X11 REQUIRED)
    find_package (LibUUID REQUIRED)
    find_package (LibICU REQUIRED)

    if (NOT X11_Xcursor_FOUND)
        message (FATAL_ERROR "Could not find XCursor library.")
    endif()

    if (NOT X11_Xrandr_FOUND)
        message (FATAL_ERROR "Could not find XRandR library.")
    endif()

    if (NOT X11_Xi_FOUND)
        message (FATAL_ERROR "Could not find Xi (XInput) library.")
    endif()
endif ()

# Third party (non-package) libraries
add_library (ThirdParty INTERFACE)
target_include_directories (ThirdParty INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Utility/ThirdParty")

# Target
add_library (tef SHARED ${TE_UTILITY_SRC} ${TE_CORE_SRC})

# Includes
target_include_directories (tef PUBLIC "Utility" "Core")

# Defines
target_compile_definitions (tef PRIVATE 
    -DTE_EXPORTS
    -DTE_CORE_EXPORTS
    -DTE_UTILITY_EXPORTS
    -DTE_ENGINE_BUILD
    $<$<CONFIG:Debug>:TE_CONFIG=TE_CONFIG_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:TE_CONFIG=TE_CONFIG_RELWITHDEBINFO>
    $<$<CONFIG:MinSizeRel>:TE_CONFIG=TE_CONFIG_MINSIZEREL>
    $<$<CONFIG:Release>:TE_CONFIG=TE_CONFIG_RELEASE>)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (tef PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (tef PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (tef PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

## External libs: Header only libraries
target_link_libraries (tef PUBLIC ThirdParty)

## External lib: NVTT
target_link_libraries(tef PUBLIC ${nvtt_LIBRARIES})

if (LINUX)
    ## External lib: X11, LibUUID, LibICU
    target_link_libraries (tef PUBLIC ${X11_LIBRARIES} ${X11_Xcursor_LIB} ${X11_Xrandr_LIB} ${X11_Xi_LIB})
    target_link_libraries (tef PUBLIC ${LibUUID_LIBRARIES})
    target_link_libraries (tef PUBLIC ${LibICU_LIBRARIES})
endif ()

## OS libs
if (WIN32)
    target_link_libraries (tef PUBLIC Winmm dinput8 xinput9_1_0 dxguid.lib)
    target_link_libraries (tef PUBLIC DbgHelp IPHLPAPI Rpcrt4)
elseif (LINUX)
    target_link_libraries(tef PUBLIC dl pthread)
endif ()

# IDE specific
set_property (TARGET tef PROPERTY FOLDER Framework)

# Install
install (
    DIRECTORY Utility
    DESTINATION include
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "Private" EXCLUDE
)

install (
    DIRECTORY Core 
    DESTINATION include
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "Private" EXCLUDE
)

install (
    DIRECTORY Engine
    DESTINATION include
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "Private" EXCLUDE
)

if (LINUX)
    install_pre_build_data(tef)
endif()

install_tef_target(tef)
