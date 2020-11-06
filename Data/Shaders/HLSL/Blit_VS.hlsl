struct VS_INPUT
{
    float3 ScreenPosition : POSITION;
    float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position = float4(IN.ScreenPosition, 1.0f);
    OUT.Texture = IN.Texture;

    return OUT;
}
