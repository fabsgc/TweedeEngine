#include "TeBlitMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    BlitParamDef gBlitParamDef;

    BlitMat::BlitMat()
    {
        _paramBuffer = gBlitParamDef.CreateBuffer();
    }

    void BlitMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
    }

    void BlitMat::Execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV, INT32 MSAACount, bool isDepth, bool isFiltered)
    {
        gBlitParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);
        gBlitParamDef.gIsDepth.Set(_paramBuffer, (isDepth) ? 1 : 0, 0);

        if (MSAACount > 1 && isDepth) _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMapMSDepth", source);
        else if (MSAACount > 1 && !isDepth) _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMapMS", source);
        else _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);

        if(isFiltered)
            _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
        else
            _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));

        Bind();
        gRendererUtility().DrawScreenQuad(area, Vector2I(1, 1), 1, flipUV);
    }
}
