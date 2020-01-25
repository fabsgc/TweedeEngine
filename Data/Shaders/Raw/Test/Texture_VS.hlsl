cbuffer ObjectConstantBuffer : register(b0)
{
    float SpecularConstant;
}

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Specular : COLOR0;
    float4 Color : COLOR1;
    float2 Texture : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position = IN.Position;
    OUT.Specular =  SpecularConstant;
    OUT.Color = IN.Color;
    OUT.Texture = IN.Texture.xy;

    return OUT;
}
