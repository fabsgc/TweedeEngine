set (TE_RENDERMAN_INC_NOFILTER
    "TeRenderManPrerequisites.h"
    "TeRenderManFactory.h"
    "TeRenderMan.h"
    "TeRenderManOptions.h"
    "TeRendererScene.h"
    "TeRendererView.h"
    "TeRendererRenderable.h"
    "TeRendererLight.h"
    "TeRenderCompositor.h"
    "TeRendererDecal.h"
    "TeRenderManIBLUtility.h"
    "TeRendererTextures.h"
    "TeShadowRendering.h"
)

set (TE_RENDERERMAN_SRC_NOFILTER
    "TeRenderManFactory.cpp"
    "TeRenderManPlugin.cpp"
    "TeRenderMan.cpp"
    "TeRendererScene.cpp"
    "TeRendererView.cpp"
    "TeRendererRenderable.cpp"
    "TeRendererLight.cpp"
    "TeRenderCompositor.cpp"
    "TeRendererDecal.cpp"
    "TeRenderManIBLUtility.cpp"
    "TeRendererTextures.cpp"
    "TeShadowRendering.cpp"
)

set (TE_RENDERMAN_INC_POSTPROCESSING
    "PostProcessing/TeBloomMat.h"
    "PostProcessing/TeFXAAMat.h"
    "PostProcessing/TeMotionBlurMat.h"
    "PostProcessing/TeSkyboxMat.h"
    "PostProcessing/TeToneMappingMat.h"
    "PostProcessing/TeSSAODownsampleMat.h"
    "PostProcessing/TeSSAOMat.h"
    "PostProcessing/TeSSAOBlurMat.h"
)

set (TE_RENDERMAN_SRC_POSTPROCESSING
    "PostProcessing/TeBloomMat.cpp"
    "PostProcessing/TeFXAAMat.cpp"
    "PostProcessing/TeMotionBlurMat.cpp"
    "PostProcessing/TeSkyboxMat.cpp"
    "PostProcessing/TeToneMappingMat.cpp"
    "PostProcessing/TeSSAODownsampleMat.cpp"
    "PostProcessing/TeSSAOMat.cpp"
    "PostProcessing/TeSSAOBlurMat.cpp"
)

source_group ("" FILES ${TE_RENDERERMAN_SRC_NOFILTER} ${TE_RENDERMAN_INC_NOFILTER})
source_group ("PostProcessing" FILES ${TE_RENDERMAN_SRC_POSTPROCESSING} ${TE_RENDERMAN_INC_POSTPROCESSING})

set (TE_RENDERMAN_SRC
    ${TE_RENDERMAN_INC_NOFILTER}
    ${TE_RENDERERMAN_SRC_NOFILTER}
    ${TE_RENDERMAN_INC_POSTPROCESSING}
    ${TE_RENDERMAN_SRC_POSTPROCESSING}
)
