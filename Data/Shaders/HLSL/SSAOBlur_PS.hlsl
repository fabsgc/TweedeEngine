#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float2 gPixelSize;
    float2 gPixelOffset;
    float gInvDepthThreshold;
    uint gHorizontal;
    float2 gPadding;
}

cbuffer PerCameraBuffer : register(b1)
{
    CameraData gCamera;
}

SamplerState BilinearSampler : register(s0);
Texture2D InputMap : register(t0);
Texture2D DepthMap : register(t1);

float4 main( PS_INPUT IN ) : SV_Target0
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

// TODO
