#include "Include/PostProcessBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    float2 gSourceDimensions;
    uint gMSAACount;
    uint gHorizontal;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

static const float weight[7] = { 0.257027, 0.2145946, 0.1516216, 0.094054, 0.046216, 0.021525, 0.009204 };

float2 ClampUv(float2 uv)
{
    if(uv.x > 1.0) uv.x = 0.991;
    if(uv.x < 0.0) uv.x = 0.001;
    if(uv.y > 1.0) uv.y = 0.991;
    if(uv.y < 0.0) uv.y = 0.001;

    return uv;
}

float4 GaussianBlur(Texture2D source, Texture2DMS<float4> sourceMS, 
    SamplerState samplerState, float2 uv)
{
    float2 textureOffset = 1.0 / gSourceDimensions;
    float3 result = TextureSampling(BilinearSampler, source, sourceMS, uv, gMSAACount).rgb * weight[0];

    if(gHorizontal == 1)
    {
        for(int i = 1; i < 7; ++i)
        {
            result += TextureSampling(BilinearSampler, source, sourceMS, 
                ClampUv(uv + float2(textureOffset.x * i, 0.0)), gMSAACount).rgb * weight[i];
            result += TextureSampling(BilinearSampler, source, sourceMS, 
                ClampUv(uv - float2(textureOffset.x * i, 0.0)), gMSAACount).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 7; ++i)
        {
            result += TextureSampling(BilinearSampler, source, sourceMS, 
                ClampUv(uv + float2(0.0, textureOffset.y * i)), gMSAACount).rgb * weight[i];
            result += TextureSampling(BilinearSampler, source, sourceMS, 
                ClampUv(uv - float2(0.0, textureOffset.y * i)), gMSAACount).rgb * weight[i];
        }
    }

    return float4(result, 1.0);
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = GaussianBlur(SourceMap, SourceMapMS, BilinearSampler, IN.Texture);
    return output;
}
