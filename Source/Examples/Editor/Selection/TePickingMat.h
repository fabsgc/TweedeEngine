#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "TeSelectionUtils.h"

namespace te
{
    /** Shader that performs GPU picking. */
    class PickingMat : public RendererMaterial<PickingMat>
    {
        RMAT_DEF(BuiltinShader::Picking);

    public:
        PickingMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera);

        /** Set gpu params for each renderable */
        void BindRenderable(const HRenderable& renderable);

    private:
        PerPickSelectFrameParamDef _perFrameParamDef;
        PerPickSelectObjectParamDef _perObjectParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}
