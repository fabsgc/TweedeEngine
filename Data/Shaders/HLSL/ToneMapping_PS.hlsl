#include "Include/PostProcess.hlsli"

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
    float4 mapped = color;

    if(!gGammaOnly)
    {
        // Exposure tone mapping
        mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-color * gExposure);
    }

    // Gamma correction 
    float power = float(1.0 / gGamma);
    mapped.x = pow(abs(mapped.x), power);
    mapped.y = pow(abs(mapped.y), power);
    mapped.z = pow(abs(mapped.z), power);
    mapped.w = 1.0;

    // Contrast
    mapped.rgb = ((mapped.rgb - 0.5f) * max(gContrast, 0)) + 0.5f;

    // Brightness
    mapped.rgb = mapped.rgb + float3(gBrightness, gBrightness, gBrightness);

    return mapped;
}
