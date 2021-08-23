#pragma once

#include "TeCorePrerequisites.h"
#include "Picking/TePickingUtils.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    /** Shader that performs Hud billboards selection. */
    class HudSelectionMat : public RendererMaterial<HudSelectionMat>
    {
        RMAT_DEF(BuiltinShader::HudSelection);

    public:
        using InstanceIter = Vector<PickingUtils::PerHudInstanceData>::iterator;

    public:
        HudSelectionMat();

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
