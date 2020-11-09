#include "TeGpuPickingMat.h"

#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"

namespace te
{
    GpuPickingMat::GpuPickingMat()
    {
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerCameraBuffer", _perCameraParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
    }

    /** Set gpu params for camera */
    void GpuPickingMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProjMatrix.Transpose());
    }

    /** Set gpu params for each renderable */
    void GpuPickingMat::BindRenderable(const HRenderable& renderable)
    {
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix().Transpose());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetColor().GetAsVector4());
    }
}
