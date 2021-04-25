#include "Include/BulletDebugBase.hlsli"

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
    float3 vert[2] = {{0, 0, 0}, {0, 0, 0}};
    uint id = (uint)IN[0].Position.w;

    [unroll(2)]
    for(int i = 0; i < 2; i++)
    {
        OUT.Position = float4(vert[i], 1.0);
        //OUT.Position = mul(gInstanceData[id].MatWorldNoScale, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        if(i == 0)
            OUT.Color = gInstanceData[id].FromColor;
        else
            OUT.Color = gInstanceData[id].ToColor;

        OutputStream.Append(OUT);
    }
}
