#define USES_PS 1

#include "Include/Shadow.hlsli"

float4 main( PS_INPUT IN, out float OUT : SV_Depth) : SV_Target0
{
    // Shadow position in clip space, plus bias, scaled to [0, 1] range
    OUT = saturate(IN.ShadowPosition * gInvDepthRange + gDepthBias);
    return 0;
}
