struct VS_INPUT
{
    float3 ScreenPosition : POSITION;
    float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float2 ScreenPosition : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float2 ScreenPosition : TEXCOORD1;
};
