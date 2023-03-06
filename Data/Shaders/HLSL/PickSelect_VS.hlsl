#include "Include/Skinning.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
    uint   gRenderType;
}

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorld;
    float4 gColor;
    uint   gHasAnimation;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 BlendWeights : BLENDWEIGHT;
    uint4  BlendIndices : BLENDINDICES;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 PositionWS : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float3x4 blendMatrix = (float3x4)0;

    if(gHasAnimation)
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);

    OUT.Position = float4(IN.Position, 1.0f);
    OUT.PositionWS = float4(IN.Position, 1.0f);
    OUT.Normal = IN.Normal;

    if(gHasAnimation)
    {
        OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0);
        OUT.PositionWS = float4(mul(blendMatrix, OUT.PositionWS), 1.0);
        OUT.Normal = mul(blendMatrix, float4(OUT.Normal, 0.0f)).xyz;
    }

    OUT.Position = mul(gMatWorld, OUT.Position);
    OUT.Position = mul(gMatViewProj, OUT.Position);
    OUT.PositionWS = mul(gMatWorld, OUT.PositionWS);
    OUT.Normal = normalize(mul(gMatWorld, float4(OUT.Normal, 0.0f))).xyz;

    OUT.Color = gColor;

    return OUT;
}
