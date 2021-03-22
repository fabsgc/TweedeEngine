#include "Include/HudBase.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
    uint   gRenderType;
}

SamplerState AnisotropicSampler : register(s0);
Texture2D MaskTexture : register(t0);

float4 main( PS_INPUT IN ) : SV_Target
{
    float4 output = (float4)0;

    if(gRenderType == RENDER_TYPE_DRAW)
    {
        float alpha = MaskTexture.Sample( AnisotropicSampler, IN.Texture ).r;
        if(alpha <= 0.5)
            discard;

        output =  float4(IN.Color.rgb, alpha);
    }
    else if(gRenderType == RENDER_TYPE_SELECTION)
    {
        output = float4(1.0, 0.5, 0.0, 1.0);
    }
    else if(gRenderType == RENDER_TYPE_PICKING)
    {
        output = IN.Color;
    }

    return output;
}
