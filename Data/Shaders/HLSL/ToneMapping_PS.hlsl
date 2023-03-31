#include "Include/PostProcess.hlsli"
#include "Include/CommonGraphics.hlsli"

// MSAA_COUNT (1, 2, 4, 8)
// GAMMA_ONLY (false, true)

cbuffer PerFrameBuffer : register(b0)
{
    float  gGamma;
    float  gExposure;
    float  gContrast;
    float  gBrightness;
}

#if MSAA_COUNT > 1
Texture2DMS<float4> SourceMap : register(t0);
#else //MSAA_COUNT
SamplerState Sampler : register(s0);
Texture2D SourceMap : register(t0);
#endif //MSAA_COUNT

Texture2D SSAOMap : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 color = (float4)0;
    float4 mapped = (float4)0;

#if MSAA_COUNT > 1
    [unroll]
    for(uint i = 0; i < MSAACount; i++)
        color += SourceMap.Load(IN.Texture, i);
^^
    color /= MSAA_COUNT;
#else
    color = SourceMap.Sample(Sampler, IN.Texture);
#endif

    color *= SSAOMap.Sample(Sampler, IN.Texture).x;
    mapped = GetGammaCorrectedColor(color, true, !GAMMA_ONLY, gGamma,
        gContrast, gBrightness, gExposure);

    return mapped;
}
