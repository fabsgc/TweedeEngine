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
}
