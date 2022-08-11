#pragma once

#include "TeCorePrerequisites.h"
#include "Picking/TePickingUtils.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    /** Shader that performs Hud billboards draw and picking. */
    class HudPickingMat : public RendererMaterial<HudPickingMat>
    {
        RMAT_DEF(BuiltinShader::HudPicking);

    public:
        using InstanceIter = Vector<PickingUtils::PerHudInstanceData>::iterator;

    public:
        HudPickingMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera, PickingUtils::RenderType renderType);

        /** A subset of all Hud elements we want to draw. We can't render more than 32 hud in a raw */
        void BindHud(const InstanceIter& begin, const InstanceIter& end);

    private:
        HTexture _hudMask;

        PickingUtils::PerPickSelectFrameParamDef _perFrameParamDef;
        PickingUtils::PerHudInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };
}
