#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Image/TeTexture.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(UINT32, gRenderType)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Vector4, gColor)
    TE_PARAM_BLOCK_END

    /** Shader that performs GPU picking. */
    class GpuPickingMat : public RendererMaterial<GpuPickingMat>
    {
        RMAT_DEF(BuiltinShader::GpuPicking);

    public:
        enum class RenderType
        {
            Selection = 0x0,
            Picking = 0x1,
        };

    public:
        GpuPickingMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera, RenderType renderType);

        /** Set gpu params for each renderable */
        void BindRenderable(const HRenderable& renderable);

    private:
        PerFrameParamDef _perFrameParamDef;
        PerObjectParamDef _perObjectParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}
