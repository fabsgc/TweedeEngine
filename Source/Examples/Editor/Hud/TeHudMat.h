#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "RenderAPI/TeVertexBuffer.h"

namespace te
{
    const UINT32 STANDARD_MAX_INSTANCED_BLOCK = 32;

    struct PerHudInstanceData
    {
        Matrix4 MatWorldNoScale;
        Vector4 Color;
        float   Type;
        Vector3 Padding;
    };

    TE_PARAM_BLOCK_BEGIN(PerHudCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerHudInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerHudInstanceData, gInstances, STANDARD_MAX_INSTANCED_BLOCK)
    TE_PARAM_BLOCK_END

    /** Shader that performs Hud billboards rendering. */
    class HudMat : public RendererMaterial<HudMat>
    {
        RMAT_DEF(BuiltinShader::Hud);

        enum class Type
        {
            Camera = 0,
            RadialLight = 1,
            DirectionalLight = 2,
            SpotLight = 3
        };

        struct Element
        {
            Matrix4 WorldNoScale;
            Type ElemType;
            Color ElemColor;
        };

        using ElementIter = Vector<Element>::iterator;

    public:
        HudMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera);

        /** A subset of all Hud elements we want to draw. We can't render more than 32 hud in a raw */
        void BindHud(const ElementIter& begin, const ElementIter& end);

    private:
        HTexture _hudMask;

        PerHudCameraParamDef _perCameraParamDef;
        PerHudInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perCameraParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };
}
