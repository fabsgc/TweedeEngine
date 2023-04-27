#include "Include/PostProcess.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float gIntensity;
}

// MSAA_COUNT (1, 2, 4, 8)

#if MSAA_COUNT > 1
Texture2DMS<float4> SourceMap : register(t0);
Texture2DMS<float4> EmissiveMap : register(t1);
#else //MSAA_COUNT
SamplerState Sampler : register(s0);
Texture2D SourceMap : register(t0);
Texture2D EmissiveMap : register(t1);
#endif //MSAA_COUNT

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 currentUV = IN.Texture;
    float4 color = (float4)0;
    float4 bloom =  (float4)0;

#if MSAA_COUNT > 1
    [unroll]
    for(uint i = 0; i < MSAA_COUNT; i++)
    {
        color += SourceMap.Load(IN.Texture, i);
        bloom += EmissiveMap.Load(IN.Texture, i);
    }

    color /= MSAA_COUNT;
    bloom /= MSAA_COUNT;
#else // MSAA_COUNT
    color =  SourceMap.Sample(Sampler, IN.Texture);
    bloom =  EmissiveMap.Sample(Sampler, IN.Texture);
#endif // MSAA_COUNT

    return color + (bloom * gIntensity);
}
