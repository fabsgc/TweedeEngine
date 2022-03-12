#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float2 gSourceDimensions;
    uint gMSAACount;
    uint gHorizontal;
    uint gNumSamples;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

static const float weight[9] = { 0.32, 0.31, 0.29, 0.22, 0.17, 0.11, 0.07, 0.04, 0.01 };

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
    uint startSample = 9 - gNumSamples;
    float2 textureOffset = 1.0 / gSourceDimensions * 2;
    float3 result = TextureSampling(BilinearSampler, source, sourceMS, uv, gMSAACount).rgb * weight[startSample];

    if(gHorizontal == 1)
    {
        for(int i = 1; i < (int)gNumSamples; ++i)
        {
            float3 color = (float3)0;
            float2 uv1 = ClampUv(uv + float2(textureOffset.x * i, 0.0), textureOffset);
            float2 uv2 = ClampUv(uv - float2(textureOffset.x * i, 0.0), textureOffset);

            color += TextureSampling(BilinearSampler, source, sourceMS, uv1, gMSAACount).rgb * weight[i + startSample];
            color += TextureSampling(BilinearSampler, source, sourceMS, uv2, gMSAACount).rgb * weight[i + startSample];

            result += color;
        }
    }
    else
    {
        for(int i = 1; i < (int)gNumSamples; ++i)
        {
            float3 color = (float3)0;
            float2 uv1 = ClampUv(uv + float2(0.0, textureOffset.y * i), textureOffset);
            float2 uv2 = ClampUv(uv - float2(0.0, textureOffset.y * i), textureOffset);

            color += TextureSampling(BilinearSampler, source, sourceMS, uv1, gMSAACount).rgb * weight[i + startSample];
            color += TextureSampling(BilinearSampler, source, sourceMS, uv2, gMSAACount).rgb * weight[i + startSample];

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
