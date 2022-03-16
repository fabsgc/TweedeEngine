#ifndef __FORWARD_PS__
#define __FORWARD_PS__

#include "Include/BRDF.hlsli"
#include "Include/CommonGraphics.hlsli"
#include "Include/CommonReflectionCubemap.hlsli"

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
    uint    UseAnisotropyDirectionMap;
    uint    DoIndirectLighting;
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
    float3x3 TBN;
    float Anisotropy;
    float3 AnisotropyDirection;
    float3 AnisotropicT;
    float3 AnisotropicB;
    float Transmission;
    float IOR;
    float3 N_clearCoat;
    float ClearCoat;
    float PClearCoatRoughness;
    float ClearCoatRoughness;
    float3 SheenColor;
    float SheenDFG;
    float SheenScaling;
    float PSheenRoughness;
    float SheenRoughness;
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
    uint   gUseSkyboxPrefilteredRadianceMap;
    uint   gSkyboxNumMips;

    float4 gSceneLightColor;

    uint   gUseGamma;
    uint   gUseToneMapping;
    float  gGamma;
    float  gExposure;
    float  gContrast;
    float  gBrightness;

    float  gSkyboxBrightness;

    float gPadding3;
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
Texture2D ClearCoatRoughnessMap : register(t9);
Texture2D ClearCoatNormalMap : register(t10);
Texture2D NormalMap : register(t11);
Texture2D ParallaxMap : register(t12);
Texture2D TransmissionMap : register(t13);
Texture2D AnisotropyDirectionMap : register(t14);
TextureCube DiffuseIrrMap : register(t15);
TextureCube PrefilteredRadianceMap : register(t16);
Texture2D PreIntegratedEnvGF : register(t17);

SamplerState AnisotropicSampler : register(s0);
SamplerState BiLinearSampler : register(s1);

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
        fCurrHeight = ParallaxMap.SampleGrad( AnisotropicSampler, vTexCurrentOffset, dx, dy ).r;

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

float3 GetReflectedVector(float3 V, float3 N, PixelData pixel)
{
    float3 R = (float3)0;

    if(pixel.Anisotropy != 0.0)
    {
        float3  anisotropyDirection = pixel.Anisotropy >= 0.0 ? pixel.AnisotropicB : pixel.AnisotropicT;
        float3  anisotropicTangent  = cross(anisotropyDirection, V);
        float3  anisotropicNormal   = cross(anisotropicTangent, anisotropyDirection);
        float   bendFactor          = abs(pixel.Anisotropy) * saturate(5.0 * pixel.PRoughness);
        float3  bentNormal          = normalize(lerp(N, anisotropicNormal, bendFactor));

        R = reflect(V, bentNormal);
    }
    else
    {
        R = reflect(V, N);
    }

    return R;
}

// With microfacet BRDF the BRDF lobe is not centered around the reflected (mirror) direction.
// Because of NoL and shadow-masking terms the lobe gets shifted toward the normal as roughness
// increases. This is called the "off-specular peak". We approximate it using this function.
float3 GetSpecularDominantDir(float3 N, float3 R, float roughness)
{
    // Note: Try this formula as well:
    //  float smoothness = 1 - roughness;
    //  return lerp(N, R, smoothness * (sqrt(smoothness) + roughness));

    float r2 = roughness * roughness;
    return normalize(lerp(N, R, (1 - r2) * (sqrt(1 - r2) + r2)));
}

// V : view vector
// N : surface normal
float3 DoDiffuseIBL(float3 V, float3 N, PixelData pixel)
{
    float3 result = (float3)0;

    if(gMaterial.DoIndirectLighting && gUseSkyboxDiffuseIrrMap)
    {
        result = DiffuseIrrMap.Sample(BiLinearSampler, N).rgb * gSkyboxBrightness * pixel.DiffuseColor;
    }

    return result;
}

// V : view vector
// N : surface normal
float3 DoSpecularIBL(float3 V, float3 N, float NoV, PixelData pixel)
{
    float3 result = (float3)0;
    float3 R = GetReflectedVector(V, N, pixel);
    //float3 specR = GetSpecularDominantDir(N, R, pixel.PRoughness); TODO strange results
    float3 specR = R;

    if(gMaterial.DoIndirectLighting && gUseSkyboxPrefilteredRadianceMap)
    {
        float skyMipLevel = MapRoughnessToMipLevel(pixel.PRoughness, gSkyboxNumMips);
        float3 specular = PrefilteredRadianceMap.SampleLevel(BiLinearSampler, R, skyMipLevel).rgb * gSkyboxBrightness;
        float2 envBRDF = pixel.DFG.rg;

        result = specular * (pixel.F0 * envBRDF.x + envBRDF.y * pixel.F90);
    }

    return result;
}

// V : view vector
// N : surface normal
LightingResult DoIBL(float3 V, float3 N, float NoV, PixelData pixel)
{
    LightingResult result = (LightingResult)0;

    if(gMaterial.DoIndirectLighting && (gUseSkyboxDiffuseIrrMap || gUseSkyboxPrefilteredRadianceMap))
    {
        result.Diffuse = DoDiffuseIBL(V, N, pixel);
        result.Specular = DoSpecularIBL(V, N, NoV, pixel);
    }

    return result;
}

float3 SheenLobe(const PixelData pixel, float NoV, float NoL, float NoH)
{
    float D = DistributionCloth(pixel.SheenRoughness, NoH);
    float V = VisibilityCloth(NoV, NoL);

    return (D * V) * pixel.SheenColor;
}

float ClearCoatLobe(const PixelData pixel, const float3 H, float NoH, float LoH, out float Fcc)
{
    // If the material has a normal map, we want to use the geometric normal
    // instead to avoid applying the normal map details to the clear coat layer
    float clearCoatNoH = saturate(dot(pixel.N_clearCoat, H));

    // clear coat specular lobe
    float D = DistributionClearCoat(pixel.ClearCoatRoughness, clearCoatNoH, H);
    float V = VisibilityClearCoat(LoH);
    float F = F_Schlick(0.04, 1.0, LoH) * pixel.ClearCoat; // fix IOR to 1.5

    Fcc = F;
    return D * V * F;
}

float3 AnisotropicLobe(const PixelData pixel, const LightData light, const float3 V, const float3 H,
    float NoV, float NoL, float NoH, float LoH)
{
    float3 L = light.Direction;
    float3 T = pixel.AnisotropicT;
    float3 B = pixel.AnisotropicB;

    float ToV = dot(T, V);
    float BoV = dot(B, V);
    float ToL = dot(T, L);
    float BoL = dot(B, L);
    float ToH = dot(T, H);
    float BoH = dot(B, H);

    // Anisotropic parameters: at and ab are the roughness along the tangent and bitangent
    // to simplify materials, we derive them from a single roughness parameter
    // Kulla 2017, "Revisiting Physically Based Shading at Imageworks"
    float at = max(pixel.Roughness * (1.0 + pixel.Anisotropy), MIN_ROUGHNESS);
    float ab = max(pixel.Roughness * (1.0 - pixel.Anisotropy), MIN_ROUGHNESS);

    // specular anisotropic BRDF
    float D = DistributionAnisotropic(at, ab, ToH, BoH, NoH);
    float Vi = VisibilityAnisotropic(pixel.Roughness, at, ab, ToV, BoV, ToL, BoL, NoV, NoL);
    float3  F = Fresnel(pixel.F0, LoH);

    return (D * Vi) * F;
}

float3 IsotropicLobe(const PixelData pixel, const LightData light, const float3 H,
    float NoV, float NoL, float NoH, float LoH)
{
    float D = Distribution(pixel.Roughness, NoH, H);
    float V = Visibility(pixel.Roughness, NoV, NoL);
    float3  F = Fresnel(pixel.F0, LoH);

    return (D * V) * F;
}

float3 SpecularLobe(const PixelData pixel, const LightData light, const float3 V, const float3 H,
    float NoV, float NoL, float NoH, float LoH)
{
    if(pixel.Anisotropy != 0.0)
        return AnisotropicLobe(pixel, light, V, H, NoV, NoL, NoH, LoH);
    else
        return IsotropicLobe(pixel, light, H, NoV, NoL, NoH, LoH);
}

float3 DiffuseLobe(const PixelData pixel, float NoV, float NoL, float LoH)
{
    return pixel.DiffuseColor * Diffuse(pixel.Roughness, NoV, NoL, LoH);
}

/**
 * Evaluates lit materials with the standard shading model. This model comprises
 * of 2 BRDFs: an optional clear coat BRDF, and a regular surface BRDF.
 *
 * Surface BRDF
 * The surface BRDF uses a diffuse lobe and a specular lobe to render both
 * dielectrics and conductors. The specular lobe is based on the Cook-Torrance
 * micro-facet model (see BRDF.hlsli for more details). In addition, the specular
 * can be either isotropic or anisotropic.
 *
 * V : view vector
 * N : surface normal
 */
float3 DoDirectLighting(float3 V, float3 N ,const PixelData pixel, float NoV, const LightData light, float lightAttenuation) 
{
    float3 L = light.Direction;
    float3 H = normalize(V + L);

    float NoL = saturate(dot(N, L));
    float NoH = saturate(dot(N, H));
    float LoH = saturate(dot(L, H));

    float3 Fr = SpecularLobe(pixel, light, V, H, NoV, NoL, NoH, LoH);
    float3 Fd = DiffuseLobe(pixel, NoV, NoL, LoH);
    Fd *= (pixel.Transmission);

    // The energy compensation term is used to counteract the darkening effect
    // at high roughness
    float3 color = Fd + Fr * pixel.EnergyCompensation;

    color *= pixel.SheenScaling;
    color += SheenLobe(pixel, NoV, NoL, NoH);

    float Fcc;
    float clearCoat = ClearCoatLobe(pixel, H, NoH, LoH, Fcc);
    float attenuation = 1.0 - Fcc;

    color *= attenuation * NoL;

    // If the material has a normal map, we want to use the geometric normal
    // instead to avoid applying the normal map details to the clear coat layer
    float clearCoatNoL = saturate(dot(pixel.N_clearCoat, light.Direction));
    color += clearCoat * clearCoatNoL;

    return (color * light.Color.rgb * light.Intensity * lightAttenuation * NoL);
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
LightingResult DoLighting(float3 V, float3 P, float3 N, PixelData pixel, 
    float2 uv, bool castLight, bool useOcclusionMap)
{
    float NoV = abs(dot(N, V)) + 1e-5;
    float occlusion = 1.0;

    LightingResult totalResult = (LightingResult)0;
    LightingResult IBLResult = DoIBL(V, N, NoV, pixel);

    if(castLight)
    {
        if(useOcclusionMap)
            occlusion = OcclusionMap.Sample(AnisotropicSampler, uv).r;

        float3 V = normalize( gCamera.ViewDir - P );

        [unroll]
        for( uint i = 0; i < gLightsNumber; ++i )
        {
            LightingResult result = (LightingResult)0;

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
