#include "TeSelectionMat.h"

#include "Components/TeCRenderable.h"
#include "Components/TeCCamera.h"

namespace te
{
    SelectionMat::SelectionMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void SelectionMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix);
        _perFrameParamDef.gViewOrigin.Set(_perFrameParamBuffer, camera->GetTransform().GetPosition());
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)SelectionUtils::RenderType::Selection);
    }

    void SelectionMat::BindRenderable(const HRenderable& renderable)
    {
        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, renderable->GetMatrix());
        _perObjectParamDef.gColor.Set(_perObjectParamBuffer, renderable->GetGameObjectColor().GetAsVector4());
        _perObjectParamDef.gHasAnimation.Set(_perObjectParamBuffer, renderable->IsAnimated() ? 1 : 0);

        if (_params->HasBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices"))
            _params->SetBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices", renderable->_getInternal()->GetBoneMatrixBuffer());
    }
}
