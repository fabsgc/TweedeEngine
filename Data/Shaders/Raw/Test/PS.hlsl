cbuffer ObjectConstantBuffer : register(b0)
{
    float SpecularConstant;
}

SamplerState AnisotropicColorSampler : register(s0)
{
    Filter = COMPARISON_ANISOTROPIC;
    MaxAnisotropy = 8;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MinLOD = 0;
    MaxLOD = FLOAT32_MAX;
};

Texture2D SpecularTexture : register(t0);

struct PS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
};

float4 main( PS_INPUT IN ) : SV_Target
{
    return IN.Color * SpecularTexture.Sample(AnisotropicColorSampler, IN.Texture, 0);
}
