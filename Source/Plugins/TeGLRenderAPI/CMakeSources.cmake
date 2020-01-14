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
)

set(TE_GLRENDERAPI_INC_WIN32
	"Win32/TeWin32RenderWindow.h"
)

set(TE_GLRENDERAPI_SRC_WIN32
	"Win32/TeWin32RenderWindow.cpp"
	"glew.cpp"
)

set(TE_GLRENDERAPI_INC_LINUX
	"Linux/TeLinuxRenderWindow.h"
)

set(TE_GLRENDERAPI_SRC_LINUX
	"Linux/TeLinuxRenderWindow.cpp"
	"glew.cpp"
)

source_group ("" FILES ${TE_GLRENDERAPI_SRC_NOFILTER} ${TE_GLRENDERAPI_INC_NOFILTER})
source_group("Win32" FILES ${TE_GLRENDERAPI_INC_WIN32} ${TE_GLRENDERAPI_SRC_WIN32})
source_group("Linux" FILES ${TE_GLRENDERAPI_INC_LINUX} ${TE_GLRENDERAPI_SRC_LINUX})

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