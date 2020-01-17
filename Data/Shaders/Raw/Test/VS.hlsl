struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Color : COLOR0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    OUT.Color = IN.Color;
    return OUT;
}