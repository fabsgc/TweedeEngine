#include "TeBlitMat.h"
#include "TeRendererUtility.h"

namespace te
{
    BlitParamDef gBlitParamDef;

    BlitMat::BlitMat()
    {
        _paramBuffer = gBlitParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void BlitMat::Execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV, INT32 MSSACount, bool isDepth)
    {
        gBlitParamDef.gMSAACount.Set(_paramBuffer, MSSACount, 0);
        gBlitParamDef.gIsDepth.Set(_paramBuffer, (isDepth) ? 1 : 0, 0);

        if (MSSACount > 1 && isDepth) _params->SetTexture("SourceMapMSDepth", source);
        else if (MSSACount > 1 && !isDepth) _params->SetTexture("SourceMapMS", source);
        else _params->SetTexture("SourceMap", source);

        Bind();
        gRendererUtility().DrawScreenQuad(area, Vector2I(1, 1), 1, flipUV);
    }
}
