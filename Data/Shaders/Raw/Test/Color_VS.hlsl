cbuffer ObjectConstantBuffer : register(b0)
{
    float SpecularConstant;
}

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Specular : COLOR0;
    float4 Color : COLOR1;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Specular : COLOR0;
    float4 Color : COLOR1;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position = IN.Position;
    OUT.Specular =  SpecularConstant;
    OUT.Color = IN.Color;

    return OUT;
}
