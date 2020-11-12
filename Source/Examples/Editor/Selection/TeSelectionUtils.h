#pragma once

#include "Renderer/TeParamBlocks.h"
#include "../TeEditorUtils.h"

const UINT32 MAX_HUD_INSTANCED_BLOCK = 32;

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerPickSelectFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
        TE_PARAM_BLOCK_ENTRY(UINT32,  gRenderType)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerPickSelectObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Vector4, gColor)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerHudInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(EditorUtils::PerHudInstanceData, gInstances, MAX_HUD_INSTANCED_BLOCK)
    TE_PARAM_BLOCK_END

    enum class SelectionRenderType
    {
        Draw = 0x0,
        Selection = 0x1,
        Picking = 0x2,
    };

    enum class HudType
    {
        Camera = 0x0,
        RadialLight = 0x1,
        DirectionalLight = 0x2,
        SpotLight = 0x3
    };
}
