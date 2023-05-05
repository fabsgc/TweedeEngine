#include "TePickingMat.h"

#include "Components/TeCRenderable.h"
#include "Components/TeCCamera.h"

namespace te
{
    PickingMat::PickingMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();
    }

    void PickingMat::Initialize()
    {
        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
    }

    void PickingMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix);
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)PickingUtils::RenderType::Picking);
    }

    void PickingMat::BindRenderable(const HRenderable& renderable)
    {
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetGameObjectColor().GetAsVector4());
        _perObjectParamDef.gHasAnimation.Set(_perObjectParamBuffer, renderable->IsAnimated() ? 1 : 0);

        if (renderable->GetMobility() != ObjectMobility::Static)
        {
            if (_params->HasBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices"))
                _params->SetBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices", renderable->GetInternal()->GetBoneMatrixBuffer());
        }
    }
}
