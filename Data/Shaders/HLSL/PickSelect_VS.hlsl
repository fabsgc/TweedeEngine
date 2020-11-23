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
    float4 BlendWeights : BLENDWEIGHT;
    uint4  BlendIndices : BLENDINDICES;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4x4 blendMatrix = (float4x4)0;

    if(gHasAnimation)
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);

    OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = mul(blendMatrix, OUT.Position);
    OUT.Position = mul(gMatWorld, OUT.Position);
    OUT.Position = mul(gMatViewProj, OUT.Position);

    OUT.Color = gColor;

    return OUT;
}
