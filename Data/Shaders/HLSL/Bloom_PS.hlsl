#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float gIntensity;
    uint gMSAACount;
}

SamplerState Sampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

Texture2D EmissiveMap : register(t2);
Texture2DMS<float4> EmissiveMapMS : register(t3);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 currentUV = IN.Texture;
    float4 color =  TextureSampling(Sampler, SourceMap, SourceMapMS,
        IN.Texture, gMSAACount);
    float4 bloom =  TextureSampling(Sampler, EmissiveMap, EmissiveMapMS,
        IN.Texture, gMSAACount);

    return color + (bloom * gIntensity);
}
