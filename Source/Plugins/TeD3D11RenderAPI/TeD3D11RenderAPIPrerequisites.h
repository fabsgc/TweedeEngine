#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#if TE_DEBUG_MODE
#	define D3D_DEBUG_INFO
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif

#include <d3d11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>

namespace te
{
    class D3D11RenderAPI;
    class D3D11RenderAPIFactory;
    class D3D11RenderWindow;
}