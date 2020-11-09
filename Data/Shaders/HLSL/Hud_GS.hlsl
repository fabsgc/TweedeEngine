#include "Include/HudBase.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
}

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorldNoScale;
    float4 gColor;
}

[maxvertexcount(4)]
void main(point VS_OUTPUT IN[1], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    float3 vert[4];

    vert[0] = IN[0].Position.xyz + float3(-0.5, -0.5, 0.0); // Get bottom left vertex
    vert[1] = IN[0].Position.xyz + float3(-0.5, 0.5, 0.0);  // Get bottom right vertex
    vert[2] = IN[0].Position.xyz + float3(0.5, -0.5, 0.0);  // Get top left vertex
    vert[3] = IN[0].Position.xyz + float3(0.5, 0.5, 0.0);   // Get top right vertex

    float2 texCoord[4];
    texCoord[0] = float2(0, 1);
    texCoord[1] = float2(1, 1);
    texCoord[2] = float2(0, 0);
    texCoord[3] = float2(1, 0);

    GS_OUTPUT OUT;

    [unroll(4)]
    for(int i = 0; i < 4; i++)
    {
        OUT.Position = float4(vert[i], 0.0);
        OUT.Position = mul(OUT.Position, gMatWorldNoScale);
        OUT.Position = mul(OUT.Position, gMatViewProj);

        OUT.Tex = texCoord[i];
        OUT.Color = IN[0].Color;

        OutputStream.Append(OUT);
    }
}
