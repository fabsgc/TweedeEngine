#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "TeSelectionUtils.h"

namespace te
{
    /** Shader that performs GPU selection rendering. */
    class SelectionMat : public RendererMaterial<SelectionMat>
    {
        RMAT_DEF(BuiltinShader::Selection);

    public:
        SelectionMat();

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
