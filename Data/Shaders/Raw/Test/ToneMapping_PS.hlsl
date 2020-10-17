#include "Include/PostProcessBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float gGamma;
    float gExposure;
    float gContrast;
    float gBrightness;
    uint  gMSAACount;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 color = TextureSampling(BilinearSampler, SourceMap, SourceMapMS,
        IN.Texture, gMSAACount);

    // Exposure tone mapping
    float4 mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-color * gExposure);
    // Gamma correction 
    float power = float(1.0 / gGamma);
    mapped.x = pow(mapped.x, power);
    mapped.y = pow(mapped.y, power);
    mapped.z = pow(mapped.z, power);
    mapped.w = 1.0;

    mapped.rgb = mapped.rgb * gContrast;
    mapped.rgb = mapped.rgb + float3(gBrightness, gBrightness, gBrightness);

    return mapped;
}
