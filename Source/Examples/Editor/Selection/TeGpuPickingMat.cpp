#include "TeGpuPickingMat.h"

#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCCamera.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    GpuPickingMat::GpuPickingMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
        _params->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
    }

    void GpuPickingMat::BindCamera(const HCamera& camera, RenderType renderType)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix.Transpose());
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)renderType);
    }

    void GpuPickingMat::BindRenderable(const HRenderable& renderable)
    {
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix().Transpose());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetColor().GetAsVector4());
    }
}
