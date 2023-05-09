#include "Include/Shadow.hlsli"

cbuffer PerShadowCubeMatrices : register(b2)
{
    float4x4 gFaceVPMatrices[6];
};

cbuffer PerShadowCubeMasks : register(b3)
{
    uint gFaceMasks[6];
};

float4 main( PS_INPUT IN ) : SV_Target0
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

// TODO Shadow
