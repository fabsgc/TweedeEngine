#include "Include/PostProcess.hlsli"

#define MAX_BLUR_SAMPLES 128

cbuffer PerFrameBuffer : register(b0)
{
    float2 gSourceDimensions;
    uint gMSAACount;
    uint gHorizontal;
    uint gNumSamples;
    float3 gPadding;
    float4 gSampleOffsets[MAX_BLUR_SAMPLES / 2];
    float4 gSampleWeights[MAX_BLUR_SAMPLES];
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

float2 ClampUv(float2 uv, float2 textureOffset)
{
    float2 clampedUv = uv;

    if(uv.x > 1.0) clampedUv.x = 1.0 - textureOffset.x;
    if(uv.x < 0.0) clampedUv.x = textureOffset.x;
    if(uv.y > 1.0 - textureOffset.y * 2) clampedUv.y = 1.0 - textureOffset.y * 2;
    if(uv.y < textureOffset.y * 2) clampedUv.y = textureOffset.y * 2;

    return clampedUv;
}

float4 GaussianBlur(Texture2D source, Texture2DMS<float4> sourceMS, 
    SamplerState samplerState, float2 uv)
{
    float2 textureOffset = 1.0 / gSourceDimensions * 2;
    float3 result = (float3)0;

    for(uint i = 0; i < gNumSamples / 2; i++)
    {
        float3 color = (float3)0;
        float2 clampedUv = (float2)0;

        {
            if(gHorizontal == 1) clampedUv = ClampUv(uv + gSampleOffsets[i].xy, textureOffset);
            else clampedUv = ClampUv(uv + gSampleOffsets[i].xy, textureOffset);

            color += TextureSampling(BilinearSampler, source, sourceMS, clampedUv, gMSAACount).rgb * gSampleWeights[i * 2].rgb;
            result += color;
        }

        {
            if(gHorizontal == 1) clampedUv = ClampUv(uv + gSampleOffsets[i + 1].zw, textureOffset);
            else clampedUv = ClampUv(uv + gSampleOffsets[i + 1].zw, textureOffset);

            color += TextureSampling(BilinearSampler, source, sourceMS, clampedUv, gMSAACount).rgb * gSampleWeights[i * 2 + 1].rgb;
            result += color;
        }
    }

    return float4(result, 1.0);
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = GaussianBlur(SourceMap, SourceMapMS, BilinearSampler, IN.Texture);
    return output;
}
