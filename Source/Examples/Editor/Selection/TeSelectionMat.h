#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Picking/TePickingUtils.h"

namespace te
{
    /** Shader that performs GPU selection rendering. */
    class SelectionMat : public RendererMaterial<SelectionMat>
    {
        RMAT_DEF(BuiltinShader::Selection);

    public:
        SelectionMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera);

        /** Set gpu params for each renderable */
        void BindRenderable(const HRenderable& renderable);

    private:
        PickingUtils::PerPickSelectFrameParamDef _perFrameParamDef;
        PickingUtils::PerPickSelectObjectParamDef _perObjectParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}
