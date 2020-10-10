# Source files and their filters
include (CMakeSources.cmake)
    
# Target
add_library (TeD3D11ImGuiAPI SHARED ${TE_D3D11IMGUIAPI_SRC})

# Defines
target_compile_definitions (TeD3D11ImGuiAPI PRIVATE -DTE_D3D11_EXPORTS)

if (WIN32)
    if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        target_compile_definitions (TeD3D11ImGuiAPI PRIVATE -DTE_WIN_SDK_7)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
        target_compile_definitions (TeD3D11ImGuiAPI PRIVATE -DTE_WIN_SDK_8)
    elseif (${CMAKE_SYSTEM_VERSION} EQUAL 10.0) # Windows 10
        target_compile_definitions (TeD3D11ImGuiAPI PRIVATE -DTE_WIN_SDK_10)
    endif ()
endif ()

# Includes
target_include_directories (TeD3D11ImGuiAPI PRIVATE "./")

# Libraries
if (TE_64BIT)
    link_directories ("$(DXSDK_DIR)Lib/x64")
else()
    link_directories ("$(DXSDK_DIR)Lib/x86")
endif()
    
## External lib: DirectX 11
target_link_libraries (TeD3D11ImGuiAPI d3d11 dxgi dxguid D3DCompiler)  
    
## Local libs
target_link_libraries (TeD3D11ImGuiAPI tef)

# IDE specific
set_property (TARGET TeD3D11ImGuiAPI PROPERTY FOLDER Plugins)

if (LINUX)
    install_pre_build_data(TeD3D11ImGuiAPI)
endif()

# Install
if(RENDER_API_MODULE MATCHES "DirectX 11" AND !WIN32)
    install_tef_target(TeD3D11ImGuiAPI)
endif()