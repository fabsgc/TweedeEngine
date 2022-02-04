#include "TeBulletDebugMat.h"
#include "Renderer/TeCamera.h"

namespace te
{
    BulletDebugMat::BulletDebugMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perInstanceParamBuffer = _perInstanceParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);
    }

    void BulletDebugMat::BindCamera(const SPtr<Camera>& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix);
        _perFrameParamDef.gViewOrigin.Set(_perFrameParamBuffer, camera->GetTransform().GetPosition());
    }

    void BulletDebugMat::BindDebugElements(const InstanceIter& begin, const InstanceIter& end)
    {
        UINT32 i = 0;
        auto iter = begin;
        for (; iter != end; iter++, i++)
            _perInstanceParamDef.gInstances.Set(_perInstanceParamBuffer, **iter, (UINT32)i);
    }
}
