float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

cbuffer FrameConstantBuffer : register(b0)
{
    matrix ViewProj;
    float3 WorldCamera;
}

cbuffer ObjectConstantBuffer : register(b1)
{
    matrix World;
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
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position.xyz = IN.Position;
    OUT.Position.w = 1.0f;
    OUT.Position = mul(OUT.Position, World);
    OUT.Position = mul(OUT.Position, ViewProj);

    OUT.Color = IN.Color;
    OUT.Normal = IN.Normal;
    OUT.Texture = FlipUV(IN.Texture);

    OUT.WorldPosition.xyz = IN.Position;
    OUT.WorldPosition.w = 1.0f;
    OUT.WorldPosition = mul(OUT.WorldPosition, World);

    return OUT;
}
