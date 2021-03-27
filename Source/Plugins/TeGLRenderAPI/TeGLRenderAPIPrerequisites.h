#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#if TE_THREAD_SUPPORT == 1
#	define GLEW_MX
#endif

#define TE_OPENGL_4_1 1
#define TE_OPENGL_4_2 1
#define TE_OPENGL_4_3 1
#define TE_OPENGL_4_4 1
#define TE_OPENGL_4_5 1
#define TE_OPENGL_4_6 1

// 3.1 is the minimum supported version for OpenGL ES
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

#if TE_THREAD_SUPPORT == 1
GLEWContext* glewGetContext();

#	if TE_PLATFORM == TE_PLATFORM_WIN32
WGLEWContext* wglewGetContext();
#	endif

#endif

// Lots of generated code in here which triggers the new VC CRT security warnings
#if !defined( _CRT_SECURE_NO_DEPRECATE )
#define _CRT_SECURE_NO_DEPRECATE
#endif

namespace te
{
    /** Translated an OpenGL error code enum to an error code string. */
    const char* te_get_gl_error_string(GLenum errorCode);

    /** Checks if there have been any OpenGL errors since the last call, and if so reports them. */
    void te_check_gl_error(const char* function, const char* file, INT32 line);

#if TE_DEBUG_MODE && (!TE_OPENGL_4_3 && !TE_OPENGLES_3_2)
    #define TE_CHECK_GL_ERROR() te_check_gl_error(__PRETTY_FUNCTION__, __FILE__,__LINE__)
#else
    #define TE_CHECK_GL_ERROR()
#endif

    class GLSupport;
    class GLContext;
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
    class GLGpuBuffer;
    class GLGpuParamBlockBuffer;
    class GLInputLayoutManager;
    class GLGLSLParamParser;
    class GLGLSLProgramFactory;
    class GLGpuProgram;
}
