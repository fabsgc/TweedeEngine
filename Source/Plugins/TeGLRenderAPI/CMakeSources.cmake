set (TE_GLRENDERAPI_INC_NOFILTER
	"TeGLRenderAPIPrerequisites.h"
	"TeGLRenderAPIFactory.h"
	"TeGLRenderAPI.h"
	"TeGLRenderWindow.h"
)

set (TE_GLRENDERAPI_SRC_NOFILTER
	"TeGLRenderAPIFactory.cpp"
	"TeGLRenderAPIPlugin.cpp"
	"TeGLRenderAPI.cpp"
	"TeGLRenderWindow.cpp"
)

source_group ("" FILES ${TE_GLRENDERAPI_SRC_NOFILTER} ${TE_GLRENDERAPI_INC_NOFILTER})

set (TE_GLRENDERAPI_SRC
	${TE_GLRENDERAPI_INC_NOFILTER}
	${TE_GLRENDERAPI_SRC_NOFILTER}
)