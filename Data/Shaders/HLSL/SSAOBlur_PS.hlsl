#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float2 gPixelSize;
    float2 gPixelOffset;
    float gInvDepthThreshold;
    float3 gPadding;
}

cbuffer PerCameraBuffer : register(b1)
{
    CameraData gCamera;
}

SamplerState Sampler : register(s0);
Texture2D InputMap : register(t0);
Texture2D DepthMap : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    const int NUM_SAMPLES = 3;
    float centerDepth = ConvertFromDeviceZ(gCamera, DepthMap.Sample(Sampler, IN.Texture).r);

    float weightedSum = 0.0f;
    float weightSum = 0.0f;

    float centerAO = InputMap.Sample(Sampler, IN.Texture).r;
    weightedSum += centerAO;
    weightSum += 1.0f;

    // Note: Consider using normals as a weight as well
    [unroll]
    for(int i = 1; i < (NUM_SAMPLES + 1); ++i)
    {
        float2 sampleUV = gPixelSize * i + IN.Texture;

        float sampleAO = InputMap.Sample(Sampler, sampleUV).r;
        float sampleDepth = ConvertFromDeviceZ(gCamera, DepthMap.Sample(Sampler, sampleUV).r);

        float weight = saturate(1.0f - abs(sampleDepth - centerDepth) * gInvDepthThreshold);
        weightedSum += sampleAO * weight;
        weightSum += weight;
    }

    [unroll]
    for(int j = -NUM_SAMPLES; j < 0; ++j)
    {
        float2 sampleUV = gPixelSize * j + IN.Texture;

        float sampleAO = InputMap.Sample(Sampler, sampleUV).r;
        float sampleDepth = ConvertFromDeviceZ(gCamera, DepthMap.Sample(Sampler, sampleUV).r);

        float weight = saturate(1.0f - abs(sampleDepth - centerDepth) * gInvDepthThreshold);
        weightedSum += sampleAO * weight;
        weightSum += weight;
    }

    return weightedSum / weightSum;
}
