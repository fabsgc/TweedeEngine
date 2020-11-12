#pragma once

#include "TeCorePrerequisites.h"
#include "TeSelectionUtils.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    /** Shader that performs Hud billboards draw and picking. */
    class HudPickingMat : public RendererMaterial<HudPickingMat>
    {
        RMAT_DEF(BuiltinShader::HudPicking);

    public:
        using InstanceIter = Vector<SelectionUtils::PerHudInstanceData>::iterator;

    public:
        HudPickingMat();

        /** Set gpu params for camera */
        void BindCamera(const HCamera& camera, SelectionUtils::RenderType renderType);

        /** A subset of all Hud elements we want to draw. We can't render more than 32 hud in a raw */
        void BindHud(const InstanceIter& begin, const InstanceIter& end);

    private:
        HTexture _hudMask;

        SelectionUtils::PerPickSelectFrameParamDef _perFrameParamDef;
        SelectionUtils::PerHudInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };
}
