float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
}

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    int gLayer;
}

cbuffer PerFrameBuffer : register(b2)
{
    float gTime;
}

cbuffer PerCallBuffer : register(b3)
{
    matrix gMatWorldViewProj;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 BiTangent : BINORMAL;
    float2 Texture : TEXCOORD0;
    float4 Color : COLOR0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection : POSITION1;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position.xyz = IN.Position;
    OUT.Position.w = 1.0f;
    OUT.Position = mul(OUT.Position, gMatWorld);
    OUT.Position = mul(OUT.Position, gMatViewProj);

    OUT.Color = IN.Color;
    OUT.Normal = normalize(mul(float4(IN.Normal, 0.0f), gMatWorld)).xyz;
    OUT.Texture = FlipUV(IN.Texture);

    OUT.WorldPosition.xyz = IN.Position;
    OUT.WorldPosition.w = 1.0f;
    OUT.WorldPosition = mul(OUT.WorldPosition, gMatWorld);

    OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);

    return OUT;
}
