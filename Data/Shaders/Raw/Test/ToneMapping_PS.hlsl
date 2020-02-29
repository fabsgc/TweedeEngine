#include "Include/PostProcessBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float gGamma;
    float gExposure;
    uint gMSAACount;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

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

    // Exposure tone mapping
    float4 mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-color * gExposure);
    // Gamma correction 
    float power = float(1.0 / gGamma);
    mapped.x = pow(mapped.x, power);
    mapped.y = pow(mapped.y, power);
    mapped.z = pow(mapped.z, power);
  
    return mapped;
}
