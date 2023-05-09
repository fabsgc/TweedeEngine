#include "Include/Shadow.hlsli"
#include "Include/Skinning.hlsli"

cbuffer PerShadowCubeMatrices : register(b2)
{
    float4x4 gFaceVPMatrices[6];
};

cbuffer PerShadowCubeMasks : register(b3)
{
    uint gFaceMasks[6];
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    return OUT;
}

// TODO Shadow
