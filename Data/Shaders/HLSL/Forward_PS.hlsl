#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"
#include "Include/BRDF.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;

    OUT.Scene.rgb = gMaterial.BaseColor.rgb;
    OUT.Scene.a = 1.0;

    OUT.Normal = ComputeNormalBuffer(float4(IN.Normal.rgb, 0.0f));

    return OUT;
}
