#include "Include/CommonGraphics.hlsli"

#ifndef __FORWARD_PS__
#define __FORWARD_PS__

// #################### DEFINES

#define MAX_LIGHTS 24

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define PARALLAX_MIN_SAMPLE 8
#define PARALLAX_MAX_SAMPLE 256

// #################### STRUCTS

struct MaterialData
{
    float4  BaseColor;
    float   Metallic;
    float   Roughness;
    float   Reflectance;
    float   Occlusion;
    float4  Emissive;
    float4  SheenColor;
    float   SheenRoughness;
    float   ClearCoat;
    float   ClearCoatRoughness;
    float   Anisotropy;
    float3  AnisotropyDirection;
    float   AlphaTreshold;
    float2  UV0Repeat;
    float2  UV0Offset;
    float   ParallaxScale;
    uint    ParallaxSamples;
    float   MicroThickness;
    float   Thickness;
    float   Transmission;
    float3  Absorption;
    uint    UseBaseColorMap;
    uint    UseMetallicMap;
    uint    UseRoughnessMap;
    uint    UseReflectanceMap;
    uint    UseOcclusionMap;
    uint    UseEmissiveMap;
    uint    UseSheenColorMap;
    uint    UseSheenRoughnessMap;
    uint    UseClearCoatMap;
    uint    UseClearCoatRoughnessMap;
    uint    UseClearCoatNormalMap;
    uint    UseNormalMap;
    uint    UseParallaxMap;
    uint    UseTransmissionMap;
    uint    UseRadianceMap;
    uint    DoIndirectLighting;
    uint    UseDiffuseIrrMap;
    uint    UseSpecularIrrMap;
    uint    Padding[2];
};

struct LightData
{
    float3 Color;
    uint   Type;
    float3 Position;
    float  Intensity;
    float3 Direction;
    float  AttenuationRadius;
    float3 SpotAngles;
    float  BoundsRadius;
    float  LinearAttenuation;
    float  QuadraticAttenuation;
    uint   CastShadows;
    float  Padding2;
};

struct LightingResult
{
    float3 Diffuse;
    float3 Specular;
};

// #################### CONSTANT BUFFERS

cbuffer PerMaterialBuffer : register(b1)
{
    MaterialData gMaterial;
}

cbuffer PerLightsBuffer : register(b2)
{
    LightData gLights[MAX_LIGHTS];
    uint      gLightsNumber;
    float3    gPadding1;
}

cbuffer PerFrameBuffer : register(b3)
{
    float  gTime;
    float  gFrameDelta;
    float2 gPadding2;

    uint   gUseSkyboxMap;
    uint   gUseSkyboxDiffuseIrrMap;
    uint   gUseSkyboxSpecularIrrMap;
    float  gSkyboxBrightness;

    float4 gSceneLightColor;

    uint   gUseGamma;
    uint   gUseToneMapping;
    float  gGamma;
    float  gExposure;
    float  gContrast;
    float  gBrightness;

    float2 gPadding3;
}

Texture2D BaseColorMap : register(t0);
Texture2D MetallicMap : register(t1);
Texture2D RoughnessMap : register(t3);
Texture2D ReflectanceMap : register(t4);
Texture2D OcclusionMap : register(t5);
Texture2D EmissiveMap : register(t6);
Texture2D SheenColorMap : register(t7);
Texture2D ClearCoatMap : register(t8);
Texture2D ClearCoatNormalMap : register(t9);
Texture2D NormalMap : register(t10);
Texture2D ParallaxMap : register(t11);
Texture2D TransmissionMap : register(t12);
TextureCube RadianceMap : register(t13);
TextureCube DiffuseIrrMap : register(t14);
TextureCube SpecularIrrMap : register(t15);

SamplerState TextureSampler : register(s0);

// #################### HELPER FUNCTIONS

float4 ComputeNormalBuffer(float4 normal)
{
    return float4(normal.xyz, 1.0);
}

float4 ComputeEmissiveBuffer(float4 color, float4 emissive)
{
    float4 result = (float4)0;
    float4 mapped = GetGammaCorrectedColor(color, (bool)gUseGamma, (bool)gUseToneMapping, gGamma, gContrast, gBrightness, gExposure);

    float brightness = dot(mapped.rgb, float3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        result = float4(color.rgb, 1.0);
    else
        result = float4(0.0, 0.0, 0.0, 1.0);

    result.rgb += emissive.rgb;
    return result;
}

float3 ExpandNormal(float3 normal)
{
    return normal * 2.0f - 1.0f;
}

float3 DoNormalMapping(float3x3 TBN, Texture2D tex, SamplerState samplerState, float2 uv)
{
    float3 normal = tex.Sample(samplerState, uv).xyz;
    normal = ExpandNormal(normal);

    // Transform normal from tangent space to view space.
    normal = mul(normal, TBN);
    return normalize(normal);
}

float3 GetColor(in bool useSRGB, in bool isSRGBColor, in float3 color)
{
    float3 convertedColor = color;

    if(useSRGB && isSRGBColor)
        convertedColor = AccurateSRGBToLinear(convertedColor);

    return convertedColor;
}

float3 DoDiffuseIBL(float3 N)
{
    float3 result = (float3)0;

    if(gMaterial.DoIndirectLighting && (gMaterial.UseDiffuseIrrMap || gUseSkyboxDiffuseIrrMap))
        result = DiffuseIrrMap.Sample(TextureSampler, N).rgb * gSkyboxBrightness;

    return result;
}

float3 DoSpecularIBL(float3 N)
{
    float3 result = (float3)0;

    // TODO PBR

    return result;
}

// N : surface normal
LightingResult DoIBL(float3 N)
{
    LightingResult result = (LightingResult)0;

    if(gMaterial.DoIndirectLighting && (gMaterial.UseDiffuseIrrMap || gUseSkyboxDiffuseIrrMap))
    {
        result.Diffuse = DoDiffuseIBL(N);
        result.Specular = DoSpecularIBL(N);
    }

    return result;
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoDirectionalLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result = (LightingResult)0;

    // TODO PBR

    return result;
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoPointLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result = (LightingResult)0;

    // TODO PBR

    return result;
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoSpotLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result = (LightingResult)0;

    // TODO PBR

    return result;
}

// P : position vector in world space
// N : normal
LightingResult DoLighting(float3 P, float3 N, bool castLight)
{
    LightingResult totalResult = (LightingResult)0;
    LightingResult IBLResult = DoIBL(N);

    if(castLight)
    {
        float3 V = normalize( gCamera.ViewDir - P );
        float NoV = abs(dot(N, V)) + 1e-5;

        [unroll]
        for( uint i = 0; i < gLightsNumber; ++i )
        {
            LightingResult result = (LightingResult)0;

            float3 L = gLights[i].Direction;
            float3 H = normalize(V + L);

            float NoL = clamp(dot(N, L), 0.0, 1.0);
            float NoH = clamp(dot(N, H), 0.0, 1.0);
            float LoH = clamp(dot(L, H), 0.0, 1.0);

            if(gLights[i].Type == DIRECTIONAL_LIGHT)
                result = DoDirectionalLight( gLights[i], V, P, N );
            else if(gLights[i].Type == POINT_LIGHT)
                result = DoPointLight( gLights[i], V, P, N );
            else if(gLights[i].Type == SPOT_LIGHT)
                result = DoSpotLight( gLights[i], V, P, N );

            totalResult.Diffuse += result.Diffuse;
            totalResult.Specular += result.Specular;
        }
    }

    totalResult.Diffuse += IBLResult.Diffuse;
    totalResult.Specular += IBLResult.Specular;

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

#endif // __FORWARD_PS__
