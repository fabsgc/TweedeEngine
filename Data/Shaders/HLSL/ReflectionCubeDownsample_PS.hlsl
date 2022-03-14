#include "Include/PostProcess.hlsli"
#include "Include/CommonGraphics.hlsli"
#include "Include/ReflectionCubemapCommon.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    uint gCubeFace;
    uint gMipLevel;
}

SamplerState BilinearSampler : register(s0);
TextureCube SourceMap : register(t0);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 scaledUV = IN.Texture * 2.0f - 1.0f;
    float3 dir = GetDirFromCubeFace(gCubeFace, scaledUV);
    float4 color = SourceMap.SampleLevel(BilinearSampler, dir, gMipLevel);

    return color;
}
