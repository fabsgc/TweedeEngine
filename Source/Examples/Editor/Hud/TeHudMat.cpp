#include "TeHudMat.h"

#include "Components/TeCCamera.h"
#include "Resources/TeResourceManager.h"
#include "Image/TeTexture.h"
#include "Importer/TeTextureImportOptions.h"

namespace te
{
    HudMat::HudMat()
    {
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perInstanceParamBuffer = _perInstanceParamDef.CreateBuffer();

        _params->SetParamBlockBuffer("PerCameraBuffer", _perCameraParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);
        _params->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->MaxMip = 4;
        textureImportOptions->Format = IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        _hudMask = gResourceManager().Load<Texture>("Data/Textures/Hud/Hud.png");
        _params->SetTexture("MaskTexture", _hudMask.GetInternalPtr());
    }

    void HudMat::BindCamera(const HCamera& camera)
    {
        const Matrix4& projectionMatrix = camera->GetProjectionMatrixRS();
        const Matrix4& viewMatrix = camera->GetViewMatrix();

        Matrix4 viewProjMatrix = projectionMatrix * viewMatrix;
        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProjMatrix.Transpose());
        _perCameraParamDef.gViewOrigin.Set(_perCameraParamBuffer, camera->GetTransform().GetPosition());
    }

    void HudMat::BindHud(const InstanceIter& begin, const InstanceIter& end)
    {
        UINT32 i = 0;   
        for (auto iter = begin; iter != end; iter++, i++)
        {
            _perInstanceParamDef.gInstances.Set(_perInstanceParamBuffer, *iter, (UINT32)i);
        }   
    }
}
