#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
        TE_PARAM_BLOCK_ENTRY(Vector4, gColor)
    TE_PARAM_BLOCK_END

    /** Shader that performs Hud billboards rendering. */
    class HudMat : public RendererMaterial<HudMat>
    {
        RMAT_DEF(BuiltinShader::Hud);

    public:
        HudMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera);

    private:
        PerCameraParamDef _perCameraParamDef;
        PerObjectParamDef _perObjectParamDef;

        SPtr<GpuParamBlockBuffer> _perCameraParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}
