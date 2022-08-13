#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float gSampleRadius;
    float gWorldSpaceRadiusMask;
    float2 gTanHalfFOV; // x - horz FOV, y - vert FOV
    float2 gRandomTileScale;
    float gCotHalfFOV;
    float gBias;
    float2 gDownsampledPixelSize;
    float2 gFadeMultiplyAdd;
    float gPower;
    float gIntensity;
    uint gUpSample;
    uint gFinalPass;
    uint gQuality;
    float3 gPadding;
}

cbuffer PerCameraBuffer : register(b1)
{
    CameraData gCamera;
}

SamplerState BilinearSampler : register(s0);
SamplerState BilinearClampedSampler : register(s1);
Texture2D DepthMap : register(t0);
Texture2D NormalsMap : register(t1);
Texture2D DownsampledAOMap : register(t2);
Texture2D SetupAOMap : register(t3);
Texture2D RandomMap : register(t4);

float4 main( PS_INPUT IN ) : SV_Target0
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

// TODO
