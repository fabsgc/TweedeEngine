#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#if TE_DEBUG_MODE
#   define D3D_DEBUG_INFO
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#   define NOMINMAX // Required to stop windows.h messing up std::min
#endif

#include "d3d9.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <dxgi1_3.h>

#if defined(TE_WIN_SDK_10)
#include <dxgi1_4.h>
#endif

namespace te
{
    class D3D11RenderAPI;
    class D3D11Driver;
    class D3D11DriverList;
    class D3D11VideoMode;
    class D3D11VideoModeList;
    class D3D11RenderAPIFactory;
    class D3D11RenderWindow;
    class D3D11HLSLProgramFactory;
    class D3D11Device;
    class D3D11InputLayoutManager;
    class D3D11DepthStencilState;
    class D3D11Mappings;
    class D3D11RenderStateManager;
    class D3D11BlendState;
    class D3D11RasterizerState;
    class D3D11SamplerState;
    class D3D11Texture;
    class D3D11TextureManager;
    class D3D11TextureView;
    class D3D11RenderTexture;
    class D3D11HLSLProgramFactory;
    class D3D11GpuProgram;
    class D3D11GpuVertexProgram;
    class D3D11GpuPixelProgram;
    class D3D11GpuDomainProgram;
    class D3D11GpuHullProgram;
    class D3D11GpuGeometryProgram;
    class D3D11GpuComputeProgram;
    class D3D11HLSLParamParser;
    class D3D11HardwareBuffer;
    class D3D11HardwareBufferManager;
    class D3D11VertexBuffer;
    class D3D11IndexBuffer;
    class D3D11InputLayoutManager;
    class D3D11GpuBuffer;
    class GpuBufferView;
}
