#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "../TeEditorUtils.h"

namespace te
{
    const UINT32 MAX_HUD_INSTANCED_BLOCK = 32;

    TE_PARAM_BLOCK_BEGIN(PerHudFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
        TE_PARAM_BLOCK_ENTRY(UINT32,  gRenderType)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerHudInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(EditorUtils::PerHudInstanceData, gInstances, MAX_HUD_INSTANCED_BLOCK)
    TE_PARAM_BLOCK_END

    /** Shader that performs Hud billboards rendering. */
    class HudPickingMat : public RendererMaterial<HudPickingMat>
    {
        RMAT_DEF(BuiltinShader::Hud);

    public:
        enum class Type
        {
            Camera = 0x0,
            RadialLight = 0x1,
            DirectionalLight = 0x2,
            SpotLight = 0x3
        };

        enum class RenderType
        {
            Draw = 0x0,
            Selection = 0x1,
            Picking = 0x1,
        };

        using InstanceIter = Vector<EditorUtils::PerHudInstanceData>::iterator;

    public:
        HudPickingMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera, RenderType renderType);

        /** A subset of all Hud elements we want to draw. We can't render more than 32 hud in a raw */
        void BindHud(const InstanceIter& begin, const InstanceIter& end);

    private:
        HTexture _hudMask;

        PerHudFrameParamDef _perFrameParamDef;
        PerHudInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };
}
