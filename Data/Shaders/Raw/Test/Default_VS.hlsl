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

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Position = IN.Position;
    OUT.Color = IN.Color * SpecularConstant;
    OUT.Texture = IN.Texture;

    return OUT;
}
