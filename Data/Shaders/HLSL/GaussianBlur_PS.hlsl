#include "Include/PostProcess.hlsli"

#define MAX_BLUR_SAMPLES 128

cbuffer PerFrameBuffer : register(b0)
{
    float2 gSourceDimensions;
    uint gNumSamples;
    uint gPadding;
    float4 gSampleOffsets[MAX_BLUR_SAMPLES/ 2];
    float4 gSampleWeights[MAX_BLUR_SAMPLES];
}

// MSAA_COUNT (1, 2, 4, 8)

#if MSAA_COUNT > 1
Texture2DMS<float4> SourceMap : register(t0);
#else //MSAA_COUNT
SamplerState Sampler : register(s0);
Texture2D SourceMap : register(t0);
#endif //MSAA_COUNT

float3 SampleColor(float2 uv, uint weightIdx)
{
    float3 color = (float3)0;

#if MSAA_COUNT > 1
    float3 sumColor = (float3)0;

    [unroll]
    for(uint j = 0; j < MSAA_COUNT; j++)
    {
        sumColor += SourceMap.Load(uv, j).rgb;
    }

    sumColor /= MSAA_COUNT;
    color += sumColor;
#else
    color += SourceMap.Sample(Sampler, uv).rgb * gSampleWeights[weightIdx].rgb;
#endif

    return color;
}

float4 GaussianBlur(float2 uv)
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
                color += SampleColor(offsettedUv, i * 2);
            }
        }

        {
            offsettedUv = uv + gSampleOffsets[i].zw;
            if(offsettedUv.x < 1.0 && offsettedUv.x > 0.0 && 
               offsettedUv.y < 1.0 && offsettedUv.y > 0.0)
            {
                color += SampleColor(offsettedUv, i * 2 + 1);
            }
        }

        result += color;
    }

    return float4(result, 1.0);
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = GaussianBlur( IN.Texture);
    return output;
}
