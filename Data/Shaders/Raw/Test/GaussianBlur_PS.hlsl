#include "Include/PostProcessBase.hlsli"

#define MAX_BLUR_SAMPLES 128

cbuffer PerFrameBuffer : register(b0)
{
    float4 gSampleOffsets[(STANDARD_MAX_BLUR_SAMPLES + 1) / 2];
    float4 gSampleWeights[STANDARD_MAX_BLUR_SAMPLES];
    uint gNumSamples;
    uint gMSAACount;
}

float4 GaussianBlur(Texture2D source, Texture2DMS<float4> sourceMS, 
    SamplerState samper, float2 uv, uint MSAACount)
{
    return (float4)0;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 currentUV = IN.Texture;

    return TextureSampling(BilinearSampler, SourceMap, SourceMapMS,
        IN.Texture, gMSAACount);
}
