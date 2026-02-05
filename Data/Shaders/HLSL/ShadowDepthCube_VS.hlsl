#include "Include/Shadow.hlsli"

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
    return VS_MAIN(IN);
}

// TODO Shadow
