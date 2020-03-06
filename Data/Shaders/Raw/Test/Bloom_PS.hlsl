#include "Include/PostProcessBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    uint gMSAACount;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

Texture2D EmissiveMap : register(t2);
Texture2DMS<float4> EmissiveMapMS : register(t3);

float4 main( PS_INPUT IN ) : SV_Target0
{
    return TextureSampling(BilinearSampler, SourceMap, SourceMapMS,
        IN.Texture, gMSAACount);
}
