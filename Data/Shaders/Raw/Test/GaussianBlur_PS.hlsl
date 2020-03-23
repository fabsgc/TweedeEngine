#include "Include/PostProcessBase.hlsli"

#define MAX_BLUR_SAMPLES 128

cbuffer PerFrameBuffer : register(b0)
{
    float4 gSampleOffsets[(STANDARD_MAX_BLUR_SAMPLES + 1) / 2];
    float4 gSampleWeights[STANDARD_MAX_BLUR_SAMPLES];
    uint gNumSamples;
    uint gMSAACount;
    uint gIsAdditive;
}

float4 GaussianBlur(Texture2D source, Texture2DMS<float4> sourceMS, 
    SamplerState samplerState, float2 uv)
{
    // Note: Consider adding a version of this shader with unrolled loop for small number of samples
    float4 output = 0;

    uint idx = 0;
    for(; idx < (gNumSamples / 4); idx++)
    {
        {
            float2 sampleUV = uv + gSampleOffsets[idx * 2 + 0].xy;
            output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 0];
        }
        
        {
            float2 sampleUV = uv + gSampleOffsets[idx * 2 + 0].zw;
            output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 1];
        }
        
        {
            float2 sampleUV = uv + gSampleOffsets[idx * 2 + 1].xy;
            output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 2];
        }
        
        {
            float2 sampleUV = uv + gSampleOffsets[idx * 2 + 1].zw;
            output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 3];
        }
    }

    uint extraSamples = gNumSamples - idx * 4;
    [branch]
    if(extraSamples >= 1)
    {
        float2 sampleUV = uv + gSampleOffsets[idx * 2 + 0].xy;
        output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 0];
        
        [branch]
        if(extraSamples >= 2)
        {
            float2 sampleUV = uv + gSampleOffsets[idx * 2 + 0].zw;
            output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 1];
            
            [branch]
            if(extraSamples >= 3)
            {
                float2 sampleUV = uv + gSampleOffsets[idx * 2 + 1].xy;
                output += TextureSampling(samplerState, source, sourceMS, uv, gMSAACount) * gSampleWeights[idx * 4 + 2];
            }
        }
    }

    return output;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

Texture2D AdditiveMap : register(t2);

float4 main( PS_INPUT IN ) : SV_Target0
{
    //float4 output = GaussianBlur(SourceMap, SourceMapMS, BilinearSampler, IN.Texture);

    float4 output = TextureSampling(BilinearSampler, SourceMap, SourceMapMS, IN.Texture, gMSAACount);


    output += AdditiveMap.Sample(BilinearSampler, IN.Texture);

    return output;
}
