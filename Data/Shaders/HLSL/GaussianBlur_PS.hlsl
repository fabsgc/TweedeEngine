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

float4 GaussianBlur(Texture2D source, Texture2DMS<float4> sourceMS, 
    SamplerState samplerState, float2 uv)
{
    float2 textureOffset = 1.0 / gSourceDimensions * 2;
    float2 offsettedUv = (float2)0;
    float3 result = (float3)0;

    uint i = 0;
    for(; i < gNumSamples / 2; i++)
    {
        float3 color = (float3)0;

        {
            offsettedUv = uv + gSampleOffsets[i].xy;
            if(offsettedUv.x < 1.0 && offsettedUv.x > 0.0 && 
               offsettedUv.y < 1.0 && offsettedUv.y > 0.0)
            {
                color += TextureSampling(BilinearSampler, source, sourceMS, offsettedUv, gMSAACount).rgb * gSampleWeights[i * 2].rgb;
            }
        }

        {
            offsettedUv = uv + gSampleOffsets[i].zw;
            if(offsettedUv.x < 1.0 && offsettedUv.x > 0.0 && 
               offsettedUv.y < 1.0 && offsettedUv.y > 0.0)
            {
                color += TextureSampling(BilinearSampler, source, sourceMS, offsettedUv, gMSAACount).rgb * gSampleWeights[i * 2 + 1].rgb;
            }
        }

        result += color;
    }

    return float4(result, 1.0);
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = GaussianBlur(SourceMap, SourceMapMS, BilinearSampler, IN.Texture);
    return output;
}
