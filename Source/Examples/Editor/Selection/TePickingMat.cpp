#include "TePickingMat.h"

#include "Components/TeCRenderable.h"
#include "Components/TeCCamera.h"

namespace te
{
    PickingMat::PickingMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
    }

    void PickingMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix.Transpose());
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)SelectionUtils::RenderType::Picking);
    }

    void PickingMat::BindRenderable(const HRenderable& renderable)
    {
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix().Transpose());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetGameObjectColor().GetAsVector4());
    }
}
