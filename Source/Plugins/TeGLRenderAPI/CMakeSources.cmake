set (TE_GLRENDERAPI_INC_NOFILTER
    "TeGLRenderAPIPrerequisites.h"
    "TeGLRenderAPIFactory.h"
    "TeGLRenderAPI.h"
    "TeGLTexture.h"
    "TeGLTextureManager.h"
    "TeGLRenderStateManager.h"
    "TeGLBlendState.h"
    "TeGLDepthStencilState.h"
    "TeGLRasterizerState.h"
    "TeGLSamplerState.h"
    "TeGLRenderTexture.h"
    "TeGLGLSLProgramFactory.h"
    "TeGLGpuProgram.h"
    "TeGLHardwareBuffer.h"
    "TeGLHardwareBufferManager.h"
    "TeGLVertexBuffer.h"
    "TeGLIndexBuffer.h"
    "TeGLGpuParamBlockBuffer.h"
    "TeGLGpuBuffer.h"
    "TeGLGLSLParamParser.h"
    "TeGLContext.cpp"
    "TeGLSupport.cpp"
    "TeGLVertexArrayObjectManager.cpp"
    "TeGLPixelFormat.cpp"
)

set (TE_GLRENDERAPI_SRC_NOFILTER
    "TeGLRenderAPIFactory.cpp"
    "TeGLRenderAPIPlugin.cpp"
    "TeGLRenderAPI.cpp"
    "TeGLTexture.cpp"
    "TeGLTextureManager.cpp"
    "TeGLRenderStateManager.cpp"
    "TeGLBlendState.cpp"
    "TeGLDepthStencilState.cpp"
    "TeGLRasterizerState.cpp"
    "TeGLSamplerState.cpp"
    "TeGLRenderTexture.cpp"
    "TeGLGLSLProgramFactory.cpp"
    "TeGLGpuProgram.cpp"
    "TeGLHardwareBuffer.cpp"
    "TeGLHardwareBufferManager.cpp"
    "TeGLVertexBuffer.cpp"
    "TeGLIndexBuffer.cpp"
    "TeGLGpuParamBlockBuffer.cpp"
    "TeGLGpuBuffer.cpp"
    "TeGLGLSLParamParser.cpp"
    "TeGLContext.h"
    "TeGLSupport.h"
    "TeGLVertexArrayObjectManager.h"
    "TeGLPixelFormat.h"
)

set(TE_GLRENDERAPI_INC_WIN32
    "Win32/TeWin32RenderWindow.h"
    "Win32/TeWin32Context.h"
    "Win32/TeWin32GLSupport.h"
    "Win32/TeWin32Prerequisites.h"
    "Win32/TeWin32VideoModeInfo.h"
)

set(TE_GLRENDERAPI_SRC_WIN32
    "Win32/TeWin32RenderWindow.cpp"
    "Win32/TeWin32Context.cpp"
    "Win32/TeWin32GLSupport.cpp"
    "Win32/TeWin32VideoModeInfo.cpp"
    "glew.cpp"
)

set(TE_GLRENDERAPI_INC_LINUX
    "Linux/TeLinuxRenderWindow.h"
    "Linux/TeLinuxContext.h"
    "Linux/TeLinuxGLSupport.h"
    "Linux/TeLinuxVideoModeInfo.h"
)

set(TE_GLRENDERAPI_SRC_LINUX
    "Linux/TeLinuxRenderWindow.cpp"
    "Linux/TeLinuxContext.cpp"
    "Linux/TeLinuxGLSupport.cpp"
    "Linux/TeLinuxVideoModeInfo.cpp"
    "glew.cpp"
)

source_group ("" FILES ${TE_GLRENDERAPI_SRC_NOFILTER} ${TE_GLRENDERAPI_INC_NOFILTER})
source_group("Win32" FILES ${TE_GLRENDERAPI_INC_WIN32} ${TE_GLRENDERAPI_SRC_WIN32})

set (TE_GLRENDERAPI_SRC
    ${TE_GLRENDERAPI_INC_NOFILTER}
    ${TE_GLRENDERAPI_SRC_NOFILTER}
)

if(WIN32)
    list(APPEND TE_GLRENDERAPI_SRC
        ${TE_GLRENDERAPI_INC_WIN32}
        ${TE_GLRENDERAPI_SRC_WIN32}
    )
elseif(LINUX)
    list(APPEND TE_GLRENDERAPI_SRC
        ${TE_GLRENDERAPI_INC_LINUX}
        ${TE_GLRENDERAPI_SRC_LINUX}
    )
endif()