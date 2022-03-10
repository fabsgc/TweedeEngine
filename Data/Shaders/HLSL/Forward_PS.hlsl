#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"
#include "Include/BRDF.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;

    float3 diffuseIBL        = float3(1.0f, 1.0f, 1.0f);
    float3 normal            = IN.Normal;

    if(gMaterial.UseIBL && (gMaterial.UseDiffuseIrrMap || gUseSkyboxDiffuseIrrMap))
        diffuseIBL = DoDiffuseIBL(normal);

    OUT.Scene.rgb = gMaterial.BaseColor.rgb * diffuseIBL;
    OUT.Scene.a = 1.0;

    OUT.Normal = ComputeNormalBuffer(float4(IN.Normal.rgb, 0.0f));

    return OUT;
}
