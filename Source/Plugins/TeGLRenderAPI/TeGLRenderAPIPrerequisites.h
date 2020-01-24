#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#define TE_OPENGL_4_1 1
#define TE_OPENGL_4_2 1
#define TE_OPENGL_4_3 1
#define TE_OPENGL_4_4 1
#define TE_OPENGL_4_5 1
#define TE_OPENGL_4_6 0

#define TE_OPENGLES_3_1 0
#define TE_OPENGLES_3_2 0

#if TE_PLATFORM == TE_PLATFORM_WIN32
#if !defined( __MINGW32__ )
#   define WIN32_LEAN_AND_MEAN
#   ifndef NOMINMAX
#       define NOMINMAX // required to stop windows.h messing up std::min
#   endif
#endif
#   include <windows.h>
#   include <wingdi.h>
#   include <GL/glew.h>
#   include <GL/wglew.h>
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   include <GL/glew.h>
#   include <GL/glxew.h>
#   include <GL/glu.h>
#   define GL_GLEXT_PROTOTYPES
#endif

namespace te
{
    class GLRenderAPI;
    class GLRenderStateManager;
    class GLBlendState;
    class GLDepthStencilState;
    class GLRasterizerState;
    class GLSamplerState;
    class GLTextureManager;
    class GLTexture;
    class GLRenderTexture;
    class GLHardwareBuffer;
    class GLHardwareBufferManager;
    class GLVertexBuffer;
    class GLIndexBuffer;
    class GLInputLayoutManager;

}