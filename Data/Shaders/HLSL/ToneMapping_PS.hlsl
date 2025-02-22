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
Texture2D SourceMap : register(t0);
#endif //MSAA_COUNT

Texture2D SSAOMap : register(t1);
SamplerState Sampler : register(s0);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 color = (float4)0;
    float4 mapped = (float4)0;

#if MSAA_COUNT > 1
    int2 uv = trunc(IN.Texture);

    [unroll]
    for(uint i = 0; i < MSAA_COUNT; i++)
    {
        color += SourceMap.Load(uv, i);
    }

    color /= MSAA_COUNT;
#else
    color = SourceMap.Sample(Sampler, IN.Texture);
    color *= SSAOMap.Sample(Sampler, IN.Texture).x;
#endif

    mapped = GetGammaCorrectedColor(color, true, !GAMMA_ONLY, gGamma,
        gContrast, gBrightness, gExposure);

    return mapped;
}
