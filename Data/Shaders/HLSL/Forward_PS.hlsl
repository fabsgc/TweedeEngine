#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"
#include "Include/BRDF.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    LightingResult lit = (LightingResult)0;

    float3 albedo            = gMaterial.BaseColor.rgb;
    float3 emissive          = gMaterial.Emissive.rgb;

    float roughness          = gMaterial.Roughness;
    float metallic           = gMaterial.Metallic;
    float reflectance        = gMaterial.Reflectance;
    float ao                 = gMaterial.AO;

    float3 normal            = IN.Normal;

    float3 sceneLightColor   = gSceneLightColor.rgb;
    float3 diffuseIBL        = float3(1.0f, 1.0f, 1.0f);
    float3 specularIBL       = (float3)0;

    bool   useSRGB           = (bool)IN.Other.z;

    if(useSRGB)
    {
        albedo = AccurateSRGBToLinear(albedo);
        emissive = AccurateSRGBToLinear(albedo);
    }

    if(gMaterial.UseIndirectLighting && (gMaterial.UseDiffuseIrrMap || gUseSkyboxDiffuseIrrMap))
        diffuseIBL = DoDiffuseIBL(normal);

    lit.Diffuse = albedo * diffuseIBL * sceneLightColor;
    lit.Specular = (float3)0 * specularIBL;

    OUT.Scene.rgb = lit.Diffuse + lit.Specular;
    OUT.Scene.a = 1.0;

    OUT.Normal = ComputeNormalBuffer(float4(normal, 0.0f));

    return OUT;
}
