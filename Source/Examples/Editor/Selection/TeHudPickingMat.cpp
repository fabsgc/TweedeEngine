#include "TeHudPickingMat.h"

#include "Image/TeTexture.h"
#include "Components/TeCCamera.h"
#include "Resources/TeResourceManager.h"
#include "Importer/TeTextureImportOptions.h"

namespace te
{
    HudPickingMat::HudPickingMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perInstanceParamBuffer = _perInstanceParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);
        _params->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->Format = IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        _hudMask = gResourceManager().Load<Texture>("Data/Textures/Hud/Hud.png", textureImportOptions);
        _params->SetTexture("MaskTexture", _hudMask.GetInternalPtr());
    }

    void HudPickingMat::BindCamera(const HCamera& camera, SelectionUtils::RenderType renderType)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perFrameParamDef.gMatViewProj.Set(_perFrameParamBuffer, viewProjMatrix.Transpose());
        _perFrameParamDef.gViewOrigin.Set(_perFrameParamBuffer, camera->GetTransform().GetPosition());
        _perFrameParamDef.gRenderType.Set(_perFrameParamBuffer, (UINT32)renderType);
    }

    void HudPickingMat::BindHud(const InstanceIter& begin, const InstanceIter& end)
    {
        UINT32 i = 0;
        for (auto iter = begin; iter != end; iter++, i++)
        {
            _perInstanceParamDef.gInstances.Set(_perInstanceParamBuffer, *iter, (UINT32)i);
        }
    }
}
