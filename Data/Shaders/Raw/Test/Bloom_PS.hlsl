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
    float4 color = (float4)0;

    if(gMSAACount > 1)
    {
        float4 sum = float4(0, 0, 0, 0);

        for(uint i = 0; i < gMSAACount; i++)
            sum += SourceMapMS.Load(IN.Texture, i);

        color = sum / gMSAACount;
    }

    color = SourceMap.Sample(BilinearSampler, IN.Texture);

    return color;
}
