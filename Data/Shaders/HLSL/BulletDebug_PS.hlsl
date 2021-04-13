#include "Include/BulletDebugBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
}

float4 main( PS_INPUT IN ) : SV_Target
{
    return IN.Color;
}
