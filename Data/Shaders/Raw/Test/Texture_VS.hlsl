#include "Include/Common.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
}

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_FORWARD_MAX_INSTANCED_BLOCK];
}

cbuffer PerObjectBuffer : register(b2)
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
}

cbuffer PerFrameBuffer : register(b3)
{
    float gTime;
}

cbuffer PerCallBuffer : register(b4)
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
    uint   Instanceid : SV_InstanceID;
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

    if(IN.Instanceid == 0)
    {
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
    }
    else
    {
        OUT.Position.xyz = IN.Position;
        OUT.Position.w = 1.0f;
        OUT.Position = mul(OUT.Position, gInstanceData[IN.Instanceid].gMatWorld);
        OUT.Position = mul(OUT.Position, gMatViewProj);

        OUT.Color = IN.Color;
        OUT.Normal = normalize(mul(float4(IN.Normal, 0.0f), gInstanceData[IN.Instanceid].gMatWorld)).xyz;
        OUT.Texture = FlipUV(IN.Texture);

        OUT.WorldPosition.xyz = IN.Position;
        OUT.WorldPosition.w = 1.0f;
        OUT.WorldPosition = mul(OUT.WorldPosition, gInstanceData[IN.Instanceid].gMatWorld);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);
    }

    return OUT;
}
