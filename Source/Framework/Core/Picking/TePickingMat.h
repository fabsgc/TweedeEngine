#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Picking/TePickingUtils.h"

namespace te
{
    /** Shader that performs GPU picking. */
    class TE_CORE_EXPORT PickingMat : public RendererMaterial<PickingMat>
    {
        RMAT_DEF(BuiltinShader::Picking);

    public:
        PickingMat();

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
