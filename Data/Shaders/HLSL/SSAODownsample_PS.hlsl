#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float2 gPixelSize;
    float gInvDepthThreshold;
}

cbuffer PerCameraBuffer : register(b1)
{
    CameraData gCamera;
}

SamplerState BilinearSampler : register(s0);
Texture2D DepthMap : register(t0);
Texture2D NormalsMap : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    uint i = 0;
    float2 uvs[4];
    uvs[0] = IN.Texture + float2(-0.5f, -0.5f) * gPixelSize;
    uvs[1] = IN.Texture + float2(-0.5f,  0.5f) * gPixelSize;
    uvs[2] = IN.Texture + float2( 0.5f, -0.5f) * gPixelSize;
    uvs[3] = IN.Texture + float2( 0.5f,  0.5f) * gPixelSize;

    float4 samples[4];
    [unroll]
    for(i = 0; i < 4; i++)
    {
        samples[i].xyz = NormalsMap.Sample(BilinearSampler, uvs[i]).xyz;
        samples[i].w = ConvertFromDeviceZ(gCamera, DepthMap.Sample(BilinearSampler, uvs[i]).r);
    }

    float maxZ = max(max(samples[0].w, samples[1].w), max(samples[2].w, samples[3].w));
		
    float3 weightedSum = 0.0f;
    float weightSum = 0.00001f; // Avoid division by 0
    [unroll]
    for(i = 0; i < 4; i++)
    {
        float weight = saturate(1.0f - abs(samples[i].w - maxZ) * gInvDepthThreshold);
    
        weightedSum += samples[i].xyz * weight;
        weightSum += weight;
    }
    
    return float4(weightedSum / weightSum, maxZ);
}

// TODO
