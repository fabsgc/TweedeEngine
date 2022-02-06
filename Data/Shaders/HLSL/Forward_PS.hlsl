#include "Include/ForwardBase.hlsli"
#include "Include/ForwardBase_PS.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;

    float3 baseColor = gMaterial.BaseColor.rgb;
    float alpha = gMaterial.BaseColor.a;
    float3 normal = IN.Normal.rgb;

    OUT.Scene.rgb = baseColor;
    OUT.Scene.a = alpha;

    OUT.Normal = ComputeNormalBuffer(float4(normal, 0.0f));

    return OUT;
}
