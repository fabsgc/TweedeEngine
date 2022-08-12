#include "TeTextureCubeDownsampleMat.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    TextureCubeDownsampleParamDef gTextureCubeDownsampleParamDef;

    TextureCubeDownsampleMat::TextureCubeDownsampleMat()
    {
        _paramBuffer = gTextureCubeDownsampleParamDef.CreateBuffer();
    }

    void TextureCubeDownsampleMat::Initialize()
    {
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void TextureCubeDownsampleMat::Execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip,
        const SPtr<RenderTarget>& target)
    {
        gTextureCubeDownsampleParamDef.gCubeFace.Set(_paramBuffer, face);

        const RenderAPICapabilities& caps = gCaps();
        if (caps.HasCapability(RSC_TEXTURE_VIEWS))
        {
            _params->SetTexture("SourceMap", source, TextureSurface(mip, 1, 0, 6));
            gTextureCubeDownsampleParamDef.gMipLevel.Set(_paramBuffer, 0);
        }
        else
        {
            _params->SetTexture("SourceMap", source);
            gTextureCubeDownsampleParamDef.gMipLevel.Set(_paramBuffer, mip);
        }

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(target);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
