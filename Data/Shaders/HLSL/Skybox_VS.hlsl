#include "Include/Common.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    CameraData gCamera;
}

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Direction : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 pos = mul(gCamera.MatViewProj, float4(IN.Position.xyz + gCamera.ViewOrigin, 1));

    // Set Z = W so that final depth is 1.0f and it renders behind everything else
    OUT.Position = pos.xyww;
    OUT.Direction = IN.Position;

    return OUT;
}
