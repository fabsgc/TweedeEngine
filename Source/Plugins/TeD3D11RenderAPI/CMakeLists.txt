# Source files and their filters
include (CMakeSources.cmake)

message("-- TeD3D11RenderAPI Initialization...")

# Target
add_library (TeD3D11RenderAPI SHARED ${TE_D3D11RENDERAPI_SRC})

# Defines
target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_D3D11_EXPORTS -DTE_ENGINE_BUILD)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeD3D11RenderAPI PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeD3D11RenderAPI PRIVATE "./")

# Libraries
if (TE_64BIT)
    link_directories ("$(DXSDK_DIR)Lib/x64")
else()
    link_directories ("$(DXSDK_DIR)Lib/x86")
endif()

## External lib: DirectX 11 (and DirectX 9 for debug purpose)
if(D3D9_FOUND EQUAL 1)
    message("-- ...Link with D3D9.")
    target_link_libraries (TeD3D11RenderAPI d3d9 d3d11 dxgi dxguid D3DCompiler)
else()
    target_link_libraries (TeD3D11RenderAPI d3d11 dxgi dxguid D3DCompiler)
endif()

## Local libs
target_link_libraries (TeD3D11RenderAPI tef)

# IDE specific
set_property (TARGET TeD3D11RenderAPI PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeD3D11RenderAPI)
endif()

# Install
if(RENDER_API_MODULE MATCHES "DirectX 11" AND !WIN32)
    install_tef_target(TeD3D11RenderAPI)
endif()

message("-- ...TeD3D11RenderAPI OK.")
