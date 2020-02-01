set (TE_RENDERMAN_INC_NOFILTER
    "TeRenderManPrerequisites.h"
    "TeRenderManFactory.h"
    "TeRenderMan.h"
    "TeRenderManOptions.h"
    "TeRendererScene.h"
    "TeRendererView.h"
    "TeRendererRenderable.h"
    "TeRendererLight.h"
)

set (TE_RENDERERMAN_SRC_NOFILTER
    "TeRenderManFactory.cpp"
    "TeRenderManPlugin.cpp"
    "TeRenderMan.cpp"
    "TeRendererScene.cpp"
    "TeRendererView.cpp"
    "TeRendererRenderable.cpp"
    "TeRendererLight.cpp"
)

source_group ("" FILES ${TE_RENDERERMAN_SRC_NOFILTER} ${TE_RENDERMAN_INC_NOFILTER})

set (TE_RENDERMAN_SRC
    ${TE_RENDERMAN_INC_NOFILTER}
    ${TE_RENDERERMAN_SRC_NOFILTER}
)