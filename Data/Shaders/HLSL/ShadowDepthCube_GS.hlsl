#define USES_GS 1

#include "Include/Shadow.hlsli"

cbuffer PerShadowCubeMatrices : register(b2)
{
    float4x4 gFaceVPMatrices[6];
};

cbuffer PerShadowCubeMasks : register(b3)
{
    uint gFaceMasks[6];
};

struct GS_OUTPUT
{
    float4 Position : SV_POSITION;
    float ShadowPosition : TEXCOORD0;
    uint RenderTargetIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    [unroll]
    for (uint face = 0; face < 6; ++face)
    {
        if (gFaceMasks[face] == 0)
            continue;

        [unroll]
        for (uint vertex = 0; vertex < 3; ++vertex)
        {
            GS_OUTPUT output;
            float4 clipPosition = mul(gFaceVPMatrices[face], input[vertex].WorldPosition);
            output.Position = clipPosition;
            output.ShadowPosition = clipPosition.z;
            output.RenderTargetIndex = face;
            outputStream.Append(output);
        }

        outputStream.RestartStrip();
    }
}
