#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeParamBlocks.h"
#include "Math/TeMatrix4.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(ShadowParamsDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector2, gNDCZToDeviceZ)
        TE_PARAM_BLOCK_ENTRY(float, gDepthBias)
        TE_PARAM_BLOCK_ENTRY(float, gInvDepthRange)
    TE_PARAM_BLOCK_END

    extern ShadowParamsDef gShadowParamsDef;

    /** Provides functionality for rendering shadow maps. */
    class ShadowRendering
    {
    public:
        ShadowRendering(UINT32 shadowMapSize);

        /** Changes the default shadow map size. Will cause all shadow maps to be rebuilt. */
        void SetShadowMapSize(UINT32 size);

    protected:
        UINT32 _shadowMapSize;
    };
}
