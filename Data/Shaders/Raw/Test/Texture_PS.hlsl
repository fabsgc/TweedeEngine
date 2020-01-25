struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Specular : COLOR0;
    float4 Color : COLOR1;
    float2 Texture : TEXCOORD0;
};

SamplerState AnisotropicSampler : register(s0);
Texture2D ColorTexture : register(t0);

float4 main( PS_INPUT IN ) : SV_Target
{
    float4 color = ColorTexture.Sample(AnisotropicSampler, IN.Texture);
    float4 specular = color * IN.Specular;

    return color;
}
