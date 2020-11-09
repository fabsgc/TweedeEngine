#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
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
        GpuPickingMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera);

        /** Set gpu params for each renderable */
        void BindRenderable(const HRenderable& renderable);

    private:
        PerCameraParamDef _perCameraParamDef;
        PerObjectParamDef _perObjectParamDef;

        SPtr<GpuParamBlockBuffer> _perCameraParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}
