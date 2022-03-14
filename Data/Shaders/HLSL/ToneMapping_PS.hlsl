#include "Include/PostProcess.hlsli"
#include "Include/CommonGraphics.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float  gGamma;
    float  gExposure;
    float  gContrast;
    float  gBrightness;
    uint   gGammaOnly;
    uint   gMSAACount;
    float2 gPadding;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 color = TextureSampling(BilinearSampler, SourceMap, SourceMapMS,
        IN.Texture, gMSAACount);

    float4 mapped = GetGammaCorrectedColor(color, true, !gGammaOnly, gGamma,
        gContrast, gBrightness, gExposure);

    return mapped;
}
