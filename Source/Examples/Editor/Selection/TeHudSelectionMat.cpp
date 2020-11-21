#include "TeHudSelectionMat.h"

#include "Components/TeCCamera.h"

namespace te
{
    HudSelectionMat::HudSelectionMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perInstanceParamBuffer = _perInstanceParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);
    }

    void HudSelectionMat::BindCamera(const HCamera& camera, SelectionUtils::RenderType renderType)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix);
        _perFrameParamDef.gViewOrigin.Set(_perFrameParamBuffer, camera->GetTransform().GetPosition());
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)renderType);
    }

    void HudSelectionMat::BindHud(const InstanceIter& begin, const InstanceIter& end)
    {
        UINT32 i = 0;
        for (auto iter = begin; iter != end; iter++, i++)
        {
            _perInstanceParamDef.gInstances.Set(_perInstanceParamBuffer, *iter, (UINT32)i);
        }
    }
}
