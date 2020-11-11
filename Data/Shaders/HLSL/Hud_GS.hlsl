#include "Include/HudBase.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
}

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_MAX_INSTANCED_BLOCK];
}

[maxvertexcount(4)]
void main(point GS_INPUT IN[1], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    float3 vert[4];

    uint id = (uint)IN[0].Position.w;

    vert[0] = IN[0].Position.xyz + float3(-0.5, -0.5, 0.0); // Get bottom left vertex
    vert[1] = IN[0].Position.xyz + float3(0.5, -0.5, 0.0);  // Get bottom right vertex
    vert[2] = IN[0].Position.xyz + float3(-0.5, 0.5, 0.0);  // Get top left vertex
    vert[3] = IN[0].Position.xyz + float3(0.5, 0.5, 0.0);   // Get top right vertex

    if(gInstanceData[id].Type == HUD_CAMERA || gInstanceData[id].Type == HUD_SPOT_LIGHT)
    {
        vert[0] = IN[0].Position.xyz + float3(0.0, -0.5, 0.5); // Get bottom left vertex
        vert[1] = IN[0].Position.xyz + float3(0.0, -0.5, -0.5);  // Get bottom right vertex
        vert[2] = IN[0].Position.xyz + float3(0.0, 0.5, 0.5);  // Get top left vertex
        vert[3] = IN[0].Position.xyz + float3(0.0, 0.5, -0.5);   // Get top right vertex
    }

    float2 texCoord[4];

    if(gInstanceData[id].Type == HUD_CAMERA)
    {
        texCoord[0] = float2(0.0, 0.5);
        texCoord[1] = float2(0.5, 0.5);
        texCoord[2] = float2(0.0, 0.0);
        texCoord[3] = float2(0.5, 0.0);
    }
    else if(gInstanceData[id].Type == HUD_RADIAL_LIGHT)
    {
        texCoord[0] = float2(0.5, 0.5);
        texCoord[1] = float2(1.0, 0.5);
        texCoord[2] = float2(0.5, 0.0);
        texCoord[3] = float2(1.0, 0.0);
    }
    else if(gInstanceData[id].Type == HUD_DIRECTIONAL_LIGHT)
    {
        texCoord[0] = float2(0.0, 1.0);
        texCoord[1] = float2(0.5, 1.0);
        texCoord[2] = float2(0.0, 0.5);
        texCoord[3] = float2(0.5, 0.5);
    }
    else if(gInstanceData[id].Type == HUD_SPOT_LIGHT)
    {
        texCoord[0] = float2(0.5, 1.0);
        texCoord[1] = float2(1.0, 1.0);
        texCoord[2] = float2(0.5, 0.5);
        texCoord[3] = float2(1.0, 0.5);
    }

    GS_OUTPUT OUT;

    [unroll(4)]
    for(int i = 0; i < 4; i++)
    {
        OUT.Position = float4(vert[i], 1.0);
        OUT.Position = mul(OUT.Position, gInstanceData[id].MatWorldNoScale);
        OUT.Position = mul(OUT.Position, gMatViewProj);

        OUT.Color = gInstanceData[id].Color;
        OUT.Texture = texCoord[i];

        OutputStream.Append(OUT);
    }

    OutputStream.RestartStrip();
}
