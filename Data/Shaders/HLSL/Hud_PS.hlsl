#include "Include/HudBase.hlsli"

SamplerState AnisotropicSampler : register(s0);
Texture2D MaskTexture : register(t0);

float4 main( PS_INPUT IN ) : SV_Target
{
    float alpha = MaskTexture.Sample( AnisotropicSampler, IN.Texture ).r;
    if(alpha <= 0.5)
        discard;

    return float4(IN.Color.rgb, alpha);
}
