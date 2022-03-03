#include "Include/Hud.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
    uint   gRenderType;
}

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_MAX_INSTANCED_BLOCK];
}

[maxvertexcount(4)]
void main(point GS_INPUT IN[1], inout TriangleStream<GS_OUTPUT> OutputStream)
{
    GS_OUTPUT OUT = (GS_OUTPUT)0;
    float3 vert[4] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    float2 texCoord[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
    uint id = (uint)IN[0].Position.w;

    if(gInstanceData[id].Type == HUD_CAMERA || 
       gInstanceData[id].Type == HUD_SPOT_LIGHT || 
       gInstanceData[id].Type == HUD_AUDIO_LISTENER)
    {
        vert[0] = IN[0].Position.xyz + float3(0.0, -0.5, 0.5);  // Get bottom left vertex
        vert[1] = IN[0].Position.xyz + float3(0.0, -0.5, -0.5); // Get bottom right vertex
        vert[2] = IN[0].Position.xyz + float3(0.0, 0.5, 0.5);   // Get top left vertex
        vert[3] = IN[0].Position.xyz + float3(0.0, 0.5, -0.5);  // Get top right vertex
    }
    else
    {
        vert[0] = IN[0].Position.xyz + float3(-0.5, -0.5, 0.0); // Get bottom left vertex
        vert[1] = IN[0].Position.xyz + float3(0.5, -0.5, 0.0);  // Get bottom right vertex
        vert[2] = IN[0].Position.xyz + float3(-0.5, 0.5, 0.0);  // Get top left vertex
        vert[3] = IN[0].Position.xyz + float3(0.5, 0.5, 0.0);   // Get top right vertex
    }

    if(gInstanceData[id].Type == HUD_CAMERA)
    {
        texCoord[0] = float2(0.0, 0.33);
        texCoord[1] = float2(0.33, 0.33);
        texCoord[2] = float2(0.0, 0.0);
        texCoord[3] = float2(0.33, 0.0);
    }
    else if(gInstanceData[id].Type == HUD_RADIAL_LIGHT)
    {
        texCoord[0] = float2(0.33, 0.33);
        texCoord[1] = float2(0.67, 0.33);
        texCoord[2] = float2(0.33, 0.0);
        texCoord[3] = float2(0.67, 0.0);
    }
    else if(gInstanceData[id].Type == HUD_DIRECTIONAL_LIGHT)
    {
        texCoord[0] = float2(0.0, 0.67);
        texCoord[1] = float2(0.33, 0.67);
        texCoord[2] = float2(0.0, 0.33);
        texCoord[3] = float2(0.33, 0.33);
    }
    else if(gInstanceData[id].Type == HUD_SPOT_LIGHT)
    {
        texCoord[0] = float2(0.33, 0.67);
        texCoord[1] = float2(0.67, 0.67);
        texCoord[2] = float2(0.33, 0.33);
        texCoord[3] = float2(0.67, 0.33);
    }
    else if(gInstanceData[id].Type == HUD_AUDIO_LISTENER)
    {
        texCoord[0] = float2(0.67, 0.33);
        texCoord[1] = float2(1.0, 0.33);
        texCoord[2] = float2(0.67, 0.0);
        texCoord[3] = float2(1.0, 0.0);
    }
    else if(gInstanceData[id].Type == HUD_AUDIO_SOURCE)
    {
        texCoord[0] = float2(0.67, 0.67);
        texCoord[1] = float2(1.0, 0.67);
        texCoord[2] = float2(0.67, 0.33);
        texCoord[3] = float2(1.0, 0.33);
    }

    [unroll(4)]
    for(int i = 0; i < 4; i++)
    {
        OUT.Position = float4(vert[i], 1.0);
        OUT.Position = mul(gInstanceData[id].MatWorldNoScale, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OUT.Color = gInstanceData[id].Color;
        OUT.Texture = texCoord[i];

        OutputStream.Append(OUT);
    }
}
