#include "Include/BRDF.hlsli"
#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    LightingResult lit = (LightingResult)0;

    bool    writeVelocity     = (bool)IN.Other.x;
    bool    castLight         = (bool)IN.Other.y;
    bool    useSRGB           = (bool)IN.Other.z;

    float3  albedo            = GetColor(useSRGB, true, gMaterial.BaseColor.rgb);
    float3  emissive          = GetColor(useSRGB, true, gMaterial.Emissive.rgb);

    float   roughness          = gMaterial.Roughness * gMaterial.Roughness;
    float   metallic           = gMaterial.Metallic;
    float   reflectance        = gMaterial.Reflectance;
    float   ao                 = gMaterial.AO;

    float3   sceneLightColor   = gSceneLightColor.rgb;

    float3   N                 = IN.Normal;
    float3   P                 = IN.PositionWS.xyz;

    float3x3 TBN               = float3x3(IN.Tangent, IN.BiTangent, N);
    float2   uv0               = (IN.UV0 * gMaterial.UV0Repeat) + gMaterial.UV0Offset;
    float2   uv1               = IN.UV1;

    lit = DoLighting(P, N, castLight);

    lit.Diffuse *= albedo * sceneLightColor + emissive;
    lit.Specular = lit.Specular;

    OUT.Scene.rgb = lit.Diffuse + lit.Specular;
    OUT.Scene.a = 1.0;

    OUT.Normal = ComputeNormalBuffer(float4(N, 0.0f));
    OUT.Emissive = ComputeEmissiveBuffer(OUT.Scene, float4(emissive, 1.0));

    return OUT;
}
