#include "Include/CommonReflectionCubemap.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    uint gMipLevel;
    float3 gPadding;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
};

SamplerState BilinearSampler : register(s0);
Texture2D SourceMap : register(t0);

float4 main( PS_INPUT IN ) : SV_Target0
{
    return SourceMap.SampleLevel(BilinearSampler, IN.Texture, gMipLevel);
}
