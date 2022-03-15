#ifndef __FORWARD_PS__
#define __FORWARD_PS__

#include "Include/CommonGraphics.hlsli"

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
    uint    DoIndirectLighting;
    uint    Padding;
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

struct PixelData
{
    float3 DiffuseColor;
    float  PRoughness;
    float  Roughness;
    float3 F0;
    float3 F90;
    float3 DFG;
    float3 EnergyCompensation;
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
Texture2D RoughnessMap : register(t2);
Texture2D ReflectanceMap : register(t3);
Texture2D OcclusionMap : register(t4);
Texture2D EmissiveMap : register(t5);
Texture2D SheenColorMap : register(t6);
Texture2D SheenRoughnessMap : register(t7);
Texture2D ClearCoatMap : register(t8);
Texture2D ClearCoatNormalMap : register(t9);
Texture2D NormalMap : register(t10);
Texture2D ParallaxMap : register(t11);
Texture2D TransmissionMap : register(t12);
TextureCube DiffuseIrrMap : register(t13);
TextureCube SpecularIrrMap : register(t14);

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

float4 ComputeVelocityBuffer(float4 position, float4 prevPosition, float alpha)
{
    float2 velocity = (float2)0;
    float4 output = float4(0.0, 0.0, 0.0, 1.0);

    if(alpha >= 1.0)
    {
        if(all(position =! prevPosition))
        {
            float2 a = float2(position.xy);
            float2 b = float2(prevPosition.xy);

            velocity = (b - a);

            if(velocity.x < -0.99) velocity.x = -0.99;
            if(velocity.y < -0.99) velocity.y = -0.99;

            if(velocity.x > 0.99) velocity.x = 0.99;
            if(velocity.y > 0.99) velocity.y = 0.99;

            velocity /= 2.0;
            velocity += 0.5;

            output.xy = velocity.xy;
        }
    }

    return output;
}

float3 ExpandNormal(float3 normal)
{
    return normal * 2.0f - 1.0f;
}

float3 GetColor(in bool useSRGB, in bool isSRGBColor, in float3 color)
{
    float3 convertedColor = color;

    if(useSRGB && isSRGBColor)
        convertedColor = AccurateSRGBToLinear(convertedColor);

    return convertedColor;
}

float3 DoNormalMapping(float3x3 TBN, Texture2D tex, SamplerState samplerState, float2 uv)
{
    float3 normal = tex.Sample(samplerState, uv).xyz;
    normal = ExpandNormal(normal);

    // Transform normal from tangent space to view space.
    normal = mul(normal, TBN);
    return normalize(normal);
}

// uv : original uv coordinates
// parallaxOffsetTS : computed in vertex shader
// numsteps : number of steps to execute (more steps lead to a more convincing effect)
// parallaxScale : scale to apply on each step
float2 DoParallaxMappingSteps(float2 uv, float2 parallaxOffsetTS, int numSteps, float parallaxScale)
{
    float2 dx = ddx( uv );
    float2 dy = ddy( uv );

    float fCurrHeight = 0.0;
    float fStepSize   = 1.0 / (float) numSteps;
    float fPrevHeight = 1.0;
    float fNextHeight = 0.0;

    int    nStepIndex = 0;
    bool   bCondition = true;

    float2 vTexOffsetPerStep = fStepSize * parallaxOffsetTS * parallaxScale;
    float2 vTexCurrentOffset = uv;
    float  fCurrentBound     = 1.0;
    float  fParallaxAmount   = 0.0;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 texOffset2 = 0;

    while ( nStepIndex < numSteps ) 
    {
        vTexCurrentOffset -= vTexOffsetPerStep;

        // Sample height map which in this case is stored in the alpha channel of the normal map:
        fCurrHeight = ParallaxMap.SampleGrad( TextureSampler, vTexCurrentOffset, dx, dy ).r;

        fCurrentBound -= fStepSize;

        if ( fCurrHeight > fCurrentBound ) 
        {
            pt1 = float2( fCurrentBound, fCurrHeight );
            pt2 = float2( fCurrentBound + fStepSize, fPrevHeight );

            texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

            nStepIndex = numSteps + 1;
        }
        else
        {
            nStepIndex++;
            fPrevHeight = fCurrHeight;
        }
    }

    float fDelta2 = pt2.x - pt2.y;
    float fDelta1 = pt1.x - pt1.y;
    float fDenominator = fDelta2 - fDelta1;

    // SM 3.0 and above requires a check for divide by zero since that operation will generate an 'Inf' number instead of 0
    [flatten]if ( fDenominator == 0.0f ) 
        fParallaxAmount = 0.0f;
    else
        fParallaxAmount = ( pt1.x * fDelta2 - pt2.x * fDelta1 ) / fDenominator;

    float2 vParallaxOffset = parallaxOffsetTS  * parallaxScale * ( 1.0 - fParallaxAmount );

    return uv - vParallaxOffset;
}

// uv : original uv coordinates
// V : view vector
// Pv : view world space position
// N : surface normal
// P : position vector in worldspace
// numSamples : used to compute number of steps to execute
// parallaxScale : scale to apply on each step
// parallaxOffsetTS : computed in vertex shader
float2 DoParallaxMapping(float2 uv, float3 V, float3 Pv, float3 N, float3 P,
    uint numSamples, float parallaxScale, float2 parallaxOffsetTS)
{
    if(numSamples < 16) numSamples = 16;
    if(numSamples > PARALLAX_MAX_SAMPLE) numSamples = 256;

    // As Parallax Occlusion Mapping has a very big cost
    // We adjust 
    float3 fragToView = ( P - Pv );
    float distance = length(fragToView);

    if(distance < 750.0f)
    {
        if(distance > 450)
            numSamples /= 24;
        else if(distance > 350)
            numSamples /= 16;
        else if(distance > 250)
            numSamples /= 8;
        else if(distance > 175)
            numSamples /= 4;
        else if(distance > 75)
            numSamples /= 2;

        if(numSamples < 16) numSamples = 16;

        // Utilize dynamic flow control to change the number of samples per ray 
        // depending on the viewing angle for the surface. Oblique angles require 
        // smaller step sizes to achieve more accurate precision for computing displacement.
        // We express the sampling rate as a linear function of the angle between 
        // the geometric normal and the view direction ray:
        int parallaxSteps = (int)lerp( numSamples, PARALLAX_MIN_SAMPLE, dot( V, N ) );
        uv = DoParallaxMappingSteps(uv, parallaxOffsetTS, parallaxSteps, parallaxScale);
    }

    return uv;
}

/**
 * Returns the reflected vector at the current shading point. The reflected vector
 * return by this function might be different from shading_reflected:
 * - For anisotropic material, we bend the reflection vector to simulate
 *   anisotropic indirect lighting
 * - The reflected vector may be modified to point towards the dominant specular
 *   direction to match reference renderings when the roughness increases
 */

float3 GetReflectedVector(float3 V, float3 N, PixelData pixelData)
{
    // TODO PBR Anisotropy

    float3 R = reflect(V, N);
    return R;
}

// V : view vector
// N : surface normal
float3 DoDiffuseIBL(float3 V, float3 N, PixelData pixelData)
{
    float3 result = (float3)0;
    float3 R = GetReflectedVector(V, N, pixelData);

    if(gMaterial.DoIndirectLighting && gUseSkyboxDiffuseIrrMap)
    {
        result = DiffuseIrrMap.Sample(TextureSampler, N).rgb * gSkyboxBrightness * pixelData.DiffuseColor;
    }

    return result;
}

// V : view vector
// N : surface normal
float3 DoSpecularIBL(float3 V, float3 N, PixelData pixelData)
{
    float3 result = (float3)0;
    float3 R = GetReflectedVector(-V, N, pixelData);

    if(gMaterial.DoIndirectLighting && gUseSkyboxSpecularIrrMap)
    {
        // TODO PBR
    }

    return result;
}

// V : view vector
// N : surface normal
LightingResult DoIBL(float3 V, float3 N, PixelData pixelData)
{
    LightingResult result = (LightingResult)0;

    if(gMaterial.DoIndirectLighting && (gUseSkyboxDiffuseIrrMap || gUseSkyboxSpecularIrrMap))
    {
        result.Diffuse = DoDiffuseIBL(V, N, pixelData);
        result.Specular = DoSpecularIBL(V, N, pixelData);
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

// V : view vector
// P : position vector in world space
// N : normal
LightingResult DoLighting(float3 V, float3 P, float3 N, PixelData pixelData, 
    float2 uv, bool castLight, bool useOcclusionMap)
{
    LightingResult totalResult = (LightingResult)0;
    LightingResult IBLResult = DoIBL(V, N, pixelData);
    float occlusion = 1.0;

    if(castLight)
    {
        if(useOcclusionMap)
            occlusion = OcclusionMap.Sample(TextureSampler, uv).r;

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

            totalResult.Diffuse += result.Diffuse * occlusion;
            totalResult.Specular += result.Specular * occlusion;
        }
    }

    totalResult.Diffuse += IBLResult.Diffuse;
    totalResult.Specular += IBLResult.Specular;

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

#endif // __FORWARD_PS__
