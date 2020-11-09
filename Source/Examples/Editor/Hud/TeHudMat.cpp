#include "TeHudMat.h"

#include "Components/TeCCamera.h"

namespace te
{
    HudMat::HudMat()
    {
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerCameraBuffer", _perCameraParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
    }

    void HudMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProjMatrix.Transpose());
        _perCameraParamDef.gViewOrigin.Set(_perCameraParamBuffer, camera->GetTransform().GetPosition());
    }
}
