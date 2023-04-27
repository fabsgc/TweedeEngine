#ifndef __FORWARD_PS__
#define __FORWARD_PS__

#include "Include/BRDF.hlsli"
#include "Include/CommonMath.hlsli"
#include "Include/CommonGraphics.hlsli"
#include "Include/CommonMaterial.hlsli"
#include "Include/CommonReflectionCubemap.hlsli"

// #################### DEFINES

#define MAX_LIGHTS 24

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define PARALLAX_MIN_SAMPLE 8
#define PARALLAX_MAX_SAMPLE 256

#define REFRACTION_TYPE_SPHERE 0
#define REFRACTION_TYPE_THIN 1

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
    float   SubsurfacePower;
    float4  SubsurfaceColor;
    float   Anisotropy;
    float3  AnisotropyDirection;
    float2  UV0Repeat;
    float2  UV0Offset;
    float   ParallaxScale;
    uint    ParallaxSamples;
    float   MicroThickness;
    float   Thickness;
    float   Transmission;
    float3  Absorption;
    uint    RefractionType;
    float   AlphaTreshold;
    float2  Padding1;
    uint    UseBaseColorMap;
    uint    UseMetallicMap;
    uint    UseRoughnessMap;
    uint    UseMetallicRoughnessMap;
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
    uint    UseOpacityMap;
    uint    UseAnisotropyDirectionMap;
    uint    DoIndirectLighting;
    uint    Padding2[2];
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
    float3 DFG_Sheen;
    float3 EnergyCompensation;
    float3x3 TBN;
    float Anisotropy;
    float3 AnisotropyDirection;
    float3 AnisotropicT;
    float3 AnisotropicB;
    float3 N_clearCoat;
    float ClearCoat;
    float PClearCoatRoughness;
    float ClearCoatRoughness;
    float3 SubsurfaceColor;
    float SubsurfacePower;
    float3 SheenColor;
    float SheenScaling;
    float PSheenRoughness;
    float SheenRoughness;
    float Transmission;
    uint  RefractionType;
    float IOR;
    float EtaRI;
    float EtaIR;
    float Thickness;
    float UThickness;
    float3 Absorption;
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
Texture2D MetallicRoughnessMap : register(t3);
Texture2D ReflectanceMap : register(t4);
Texture2D OcclusionMap : register(t5);
Texture2D EmissiveMap : register(t6);
Texture2D SheenColorMap : register(t7);
Texture2D SheenRoughnessMap : register(t8);
Texture2D ClearCoatMap : register(t9);
Texture2D ClearCoatRoughnessMap : register(t10);
Texture2D ClearCoatNormalMap : register(t11);
Texture2D NormalMap : register(t12);
Texture2D ParallaxMap : register(t13);
Texture2D TransmissionMap : register(t14);
Texture2D OpacityMap : register(t15);
Texture2D AnisotropyDirectionMap : register(t16);
TextureCube DiffuseIrrMap : register(t17);
TextureCube PrefilteredRadianceMap : register(t18);
Texture2D PreIntegratedEnvGF : register(t19);

SamplerState AnisotropicSampler : register(s0);
SamplerState BilinearSampler : register(s1);
SamplerState NoFilterSampler : register(s2);

// #################### HELPER FUNCTIONS

float4 ComputeNormalBuffer(float3 normal)
{
    normal.x = normal.x * 0.5 + 0.5;
    normal.y = normal.y * 0.5 + 0.5;
    normal.z = normal.z * 0.5 + 0.5;

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

#if WRITE_VELOCITY == 1
float2 ComputeVelocityBuffer(float2 NDCPos, float2 PrevNDCPos, float alpha)
{
    float2 velocity = NDCPos - PrevNDCPos;
    velocity = EncodeVelocity16SNORM(velocity);

    return velocity;
}
#endif // WRITE_VELOCITY

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

#if USE_NORMAL_MAP == 1
float3 DoNormalMapping(float3x3 TBN, Texture2D tex, SamplerState samplerState, float2 uv)
{
    float3 normal = tex.Sample(samplerState, uv).xyz;
    normal = ExpandNormal(normal);

    // Transform normal from tangent space to view space.
    normal = mul(normal, TBN);
    return normalize(normal);
}
#endif // USE_NORMAL_MAP

#if USE_PARALLAX_MAP == 1
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
#endif // USE_PARALLAX_MAP

#if USE_PARALLAX_MAP == 1
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
        int parallaxSteps = (int)lerp( numSamples, PARALLAX_MIN_SAMPLE, ClampNoV(dot( -V, N )) );
        uv = DoParallaxMappingSteps(uv, parallaxOffsetTS, parallaxSteps, parallaxScale);
    }

    return uv;
}
#endif // USE_PARALLAX_MAP

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

        R = reflect(-V, bentNormal);
    }
    else
    {
        R = reflect(-V, N);
    }

    return R;
}

// N is the normal direction
// V is the view vector
// NdotV is the cosine angle between the view vector and the normal
float3 GetDiffuseDominantDir ( float3 V , float3 N , float NdotV , float roughness )
{
    float a = 1.02341 * roughness - 1.51174;
    float b = -0.511705 * roughness + 0.755868;
    float lerpFactor = saturate (( NdotV * a + b) * roughness ) ;
    // The result is not normalized as we fetch in a cubemap
    return lerp (N , V , lerpFactor );
}

// With microfacet BRDF the BRDF lobe is not centered around the reflected (mirror) direction.
// Because of NoL and shadow-masking terms the lobe gets shifted toward the normal as roughness
// increases. This is called the "off-specular peak". We approximate it using this function.
float3 GetSpecularDominantDir(float3 N, float3 R, float roughness)
{
    // Note: Try this formula as well:
    //  float smoothness = 1 - roughness;
    //  return lerp(N, R, smoothness * (sqrt(smoothness) + roughness));

    // Strange results

    float r2 = roughness * roughness;
    return normalize(lerp(N, R, (1 - r2) * (sqrt(1 - r2) + r2)));
}

float3 PrefilteredRadiance(const float3 R, float pRoughness)
{
    float mipLevel =  gSkyboxNumMips * pRoughness * (1.7 - 0.7 * pRoughness);
    //float skyMipLevel = gSkyboxNumMips * MapRoughnessToMipLevel(pRoughness, gSkyboxNumMips);
    return PrefilteredRadianceMap.SampleLevel(BilinearSampler, R, mipLevel).rgb * gSkyboxBrightness;
}

float3 PrefilteredRadiance(const float3 R, float pRoughness, float offset)
{
    float mipLevel =  gSkyboxNumMips * pRoughness * (1.7 - 0.7 * pRoughness);
    //float skyMipLevel = gSkyboxNumMips * MapRoughnessToMipLevel(pRoughness, gSkyboxNumMips);
    return PrefilteredRadianceMap.SampleLevel(BilinearSampler, R, mipLevel  + offset).rgb * gSkyboxBrightness;
}

float ComputeSpecularOcclusion(float NoV, float occlusion, float roughness) 
{
    return clamp(pow(NoV + occlusion, exp2(-16.0 * roughness - 1.0)) - 1.0 + occlusion, 0.0, 1.0);
}

float3 PreIntEnvGF(float NoV, float roughness)
{
    return PreIntegratedEnvGF.SampleLevel(NoFilterSampler, float2(saturate(NoV), roughness), 0).xyz;
}

float3 SpecularDFG(const PixelData pixel)
{
    return lerp(pixel.DFG.xxx, pixel.DFG.yyy, pixel.F0);
}

float3 Irradiance_RoughnessOne(const float3 N)
{
    return PrefilteredRadianceMap.SampleLevel(BilinearSampler, N, (gSkyboxNumMips - 1)).rgb * gSkyboxBrightness;
}

struct Refraction
{
    float3 Position;
    float3 Direction;
    float D;
};

Refraction RefractionSolidSphere(const PixelData pixel,const float3 V, const float3 P, const float3 N)
{
    Refraction ray = (Refraction)0;

    float3 R = refract(V, N, pixel.EtaIR);
    float NoR = dot(N, R);
    float D = pixel.Thickness * -NoR;
    ray.Position = float3(P + R * D);
    ray.D = D;
    float3 N1 = normalize(NoR * R - N * 0.5);
    ray.Direction = refract(R, N1,  pixel.EtaRI);

    return ray;
}

Refraction RefractionThinSphere(const PixelData pixel, const float3 V, const float3 P, const float3 N)
{
    Refraction ray = (Refraction)0;
    float D = 0.0;

    if(pixel.UThickness != 0.0)
    {
        float3 RR = refract(V, N, pixel.EtaIR);
        float NoR = dot(N, RR);
        D = pixel.UThickness / max(-NoR, 0.001);
        ray.Position = float3(P + RR * D);
    }
    else
    {
        ray.Position = P;
    }

    ray.Direction = V;
    ray.D = D;

    return ray;
}

float3 EvaluateRefraction(const PixelData pixel, const float3 V, const float3 P, const float3 N, float3 E)
{
    Refraction ray = (Refraction)0;
    float3 Ft = (float3)0;
    float3 T = (float3)0;
    bool hasAbsorption = false;

    if(pixel.RefractionType == REFRACTION_TYPE_SPHERE)
        ray = RefractionSolidSphere(pixel, -V, P, N);
    else
        ray = RefractionThinSphere(pixel, -V, P, N);

    if(pixel.Absorption.x != 0.0 || pixel.Absorption.y != 0.0 || pixel.Absorption.z != 0.0)
        hasAbsorption = true;

    // compute transmission T
    if(hasAbsorption)
    {
        if(pixel.Thickness != 0.0 || pixel.UThickness != 0.0)
            T = min(float3_splat(1.0), exp(-pixel.Absorption * ray.D));
        else
            T = 1.0 - pixel.Absorption;
    }

    float perceptualRoughness = lerp(pixel.PRoughness, 0.0, saturate(pixel.EtaIR * 3.0 - 2.0));

    if(pixel.RefractionType == REFRACTION_TYPE_THIN)
    {
        // For thin surfaces, the light will bounce off at the second interface in the direction of
        // the reflection, effectively adding to the specular, but this process will repeat itself.
        // Each time the ray exits the surface on the front side after the first bounce,
        // it's multiplied by E^2, and we get: E + E(1-E)^2 + E^3(1-E)^2 + ...
        // This infinite series converges and is easy to simplify.
        // Note: we calculate these bounces only on a single component,
        // since it's a fairly subtle effect.

        E *= 1.0 + pixel.Transmission * (1.0 - E.g) / (1.0 + E.g);
    }

    // when reading from the cubemap, we are not pre-exposed so we apply iblLuminance
    // which is not the case when we'll read from the screen-space buffer
    Ft = PrefilteredRadiance(ray.Direction, perceptualRoughness);

    // base color changes the amount of light passing through the boundary
    Ft *= pixel.DiffuseColor;

    // apply absorption
    if(hasAbsorption)
        Ft *= T;

    return Ft;
}

#if DO_INDIRECT_LIGHTING == 1
// V : view vector
// N : surface normal
// E : SpecularDFG
float3 DoDiffuseIBL(float3 V, float3 N, float NoV, float3 E, const PixelData pixel, float occlusion)
{
    float3 result = (float3)0;
    float3 dominantN = GetDiffuseDominantDir (V, N, NoV, pixel.Roughness);

    float3 irradiance = DiffuseIrrMap.Sample(BilinearSampler, dominantN).rgb * gSkyboxBrightness;
    result = irradiance * pixel.DiffuseColor *  occlusion;

    return result;
}
#endif // DO_INDIRECT_LIGHTING

#if DO_INDIRECT_LIGHTING == 1
// V : view vector
// N : surface normal
// E : SpecularDFG
float3 DoSpecularIBL(float3 V, float3 N, float NoV, float3 E, const PixelData pixel, float occlusion)
{
    float3 result = (float3)0;
    float3 R = GetReflectedVector(V, N, pixel);
    float3 dominantR = R;
    float ao = ComputeSpecularOcclusion(NoV, occlusion, pixel.Roughness);

    float3 radiance = PrefilteredRadiance(dominantR, pixel.PRoughness);
    float3 FssEss = pixel.F0 * pixel.DFG.x + pixel.DFG.y * pixel.F90; // E
    //result = radiance * FssEss * ao;
    result = radiance * FssEss * ao;

    // multi scattering https://google.github.io/filament/Filament.html#listing_multiscatteriblevaluation

    return result;
}
#endif // DO_INDIRECT_LIGHTING

#if DO_INDIRECT_LIGHTING == 1
// V : view vector
// N : surface normal
LightingResult DoSheenIBL(float3 V, float3 N, const PixelData pixel, float NoV, float occlusion, LightingResult result)
{
    // Albedo scaling of the base layer before we layer sheen on top
    result.Diffuse *= pixel.SheenScaling;
    result.Specular *= pixel.SheenScaling;

    float3 R = GetReflectedVector(V, N, pixel);
    float3 dominantR = R;
    float ao = ComputeSpecularOcclusion(NoV, occlusion, pixel.SheenRoughness);
    float3 reflectance = pixel.DFG_Sheen.z * pixel.SheenColor;

    float3 radiance = PrefilteredRadiance(dominantR, pixel.PSheenRoughness);

    result.Specular += reflectance * radiance * ao;

    return result;
}
#endif // DO_INDIRECT_LIGHTING

#if DO_INDIRECT_LIGHTING == 1
// V : view vector
// N : surface normal
LightingResult DoClearCoatIBL(float3 V, float3 N, const PixelData pixel, float NoV, float occlusion, LightingResult result)
{
    float3 NClearCoat = pixel.N_clearCoat;
    float clearCoatNoV = ClampNoV(dot(NClearCoat, V));
    float3 clearCoatR = GetReflectedVector(V, NClearCoat, pixel);
    float ao = ComputeSpecularOcclusion(clearCoatNoV, occlusion, pixel.ClearCoatRoughness);

    float Fc = F_Schlick(0.04, 1.0, NoV) * pixel.ClearCoat;
    float attenuation = 1.0 - Fc;
    result.Diffuse *= attenuation;
    result.Specular *= attenuation;

    float3 radiance = PrefilteredRadiance(clearCoatR, pixel.PClearCoatRoughness);

    result.Specular += Fc * radiance * ao;

    return result;
}
#endif // DO_INDIRECT_LIGHTING

/*LightingResult DoSubSurfaceIBL(float3 V, float3 N, const PixelData pixel, float NoV, float occlusion, LightingResult result)
{
    float3 viewIndependent = Irradiance_RoughnessOne(N);
    float3 viewDependent = PrefilteredRadiance(-V, pixel.Roughness, 1.0 + pixel.Thickness);
    float attenuation = (1.0 - pixel.Thickness) / (2.0 * PI);
    result.Diffuse += pixel.SubsurfaceColor * (viewIndependent + viewDependent) * attenuation;

    return result;
} TODO */

#if DO_INDIRECT_LIGHTING == 1
// V : view vector
// P : World Space position
// N : surface normal
LightingResult DoIBL(float3 V, float3 P, float3 N, float NoV, const PixelData pixel, float occlusion)
{
    float3 E = SpecularDFG(pixel);
    LightingResult result = (LightingResult)0;

    if(gMaterial.DoIndirectLighting && (gUseSkyboxDiffuseIrrMap || gUseSkyboxPrefilteredRadianceMap))
    {
        // Diffuse Layer
        result.Diffuse = DoDiffuseIBL(V, N, NoV, E, pixel, occlusion);
        result.Specular = DoSpecularIBL(V, N, NoV, E, pixel, occlusion);

        // Sheen Layer
        result = DoSheenIBL(V, N, pixel, NoV, occlusion, result);

        // Clear Coat Layer
        result = DoClearCoatIBL(V, N, pixel, NoV, occlusion, result);

        // SubSurface Layer
        // result = DoSubSurfaceIBL(V, N, pixel, NoV, occlusion, result);

        if(pixel.Transmission != 0.0)
        {
            // Refraction
            float3 Ft = EvaluateRefraction(pixel, V, P, N, E);
            result.Diffuse = result.Diffuse * (1 - pixel.Transmission) + (Ft * pixel.Transmission);
        }
    }

    return result;
}
#endif // DO_INDIRECT_LIGHTING

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
LightingResult DoLighting(float3 V, float3 P, float3 N, const PixelData pixel, 
    float2 uv, bool castLight, float occlusion)
{
    float NoV = ClampNoV(dot(N, V));

    LightingResult totalResult = (LightingResult)0;
#if DO_INDIRECT_LIGHTING == 1
    LightingResult IBLResult = DoIBL(V, P, N, NoV, pixel, occlusion);
#endif // DO_INDIRECT_LIGHTING

    if(castLight)
    {
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

            totalResult.Diffuse += result.Diffuse;
            totalResult.Specular += result.Specular;
        }
    }

#if DO_INDIRECT_LIGHTING == 1
    totalResult.Diffuse += IBLResult.Diffuse;
    totalResult.Specular += IBLResult.Specular;
#endif // DO_INDIRECT_LIGHTING

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

#endif // __FORWARD_PS__
