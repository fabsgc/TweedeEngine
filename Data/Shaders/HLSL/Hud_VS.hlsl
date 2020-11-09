#include "Include/HudBase.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
}

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorldNoScale;
    float4 gColor;
}

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position.xyz = IN.Position;
    OUT.Position.w = 1.0f;
    OUT.Position = mul(OUT.Position, gMatWorldNoScale);
    OUT.Position = mul(OUT.Position, gMatViewProj);

    OUT.Color = gColor;

    return OUT;
}
