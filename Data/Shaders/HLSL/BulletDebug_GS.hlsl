#include "Include/BulletDebug.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
}

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_MAX_INSTANCED_BLOCK];
}

[maxvertexcount(2)]
void main(point GS_INPUT IN[1], inout LineStream<GS_OUTPUT> OutputStream)
{
    GS_OUTPUT OUT = (GS_OUTPUT)0;
    uint id = (uint)IN[0].Position.w;

    [unroll(2)]
    for(int i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            OUT.Position.xyz = gInstanceData[id].From.xyz;
            OUT.Color = gInstanceData[id].FromColor;
        }
        else
        {
            OUT.Position.xyz = gInstanceData[id].To.xyz;
            OUT.Color = gInstanceData[id].ToColor;
        }

        OUT.Position.w = 1.0f;
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OutputStream.Append(OUT);
    }
}
