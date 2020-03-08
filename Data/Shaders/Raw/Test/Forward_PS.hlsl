#include "Include/ForwardBase.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
    matrix gMatPrevViewProj;
    matrix gNDCToPrevNDC;
}

cbuffer PerMaterialBuffer : register(b1)
{
    float4 gAmbient;
    float4 gDiffuse;
    float4 gEmissive;
    float4 gSpecular;
    uint   gUseDiffuseMap;
    uint   gUseEmissiveMap;
    uint   gUseNormalMap;
    uint   gUseSpecularMap;
    uint   gUseBumpMap;
    uint   gUseParallaxMap;
    uint   gUseTransparencyMap;
    uint   gUseReflectionMap;
    uint   gUseOcclusionMap;
    float  gSpecularPower;
    float  gTransparency;
    float  gIndexOfRefraction;
    float  gReflection;
    float  gAbsorbance;
    float  gBumpScale;
    float  gAlphaThreshold;
};

cbuffer PerLightsBuffer : register(b2)
{
    LightData gLights[MAX_LIGHTS];
    uint gLightsNumber;
}

cbuffer PerFrameBuffer : register(b3)
{
    float gTime;
}

SamplerState AnisotropicSampler : register(s0);

Texture2D DiffuseMap : register(t0);
Texture2D EmissiveMap : register(t1);
Texture2D NormalMap : register(t2);
Texture2D SpecularMap : register(t3);
Texture2D BumpMap : register(t4);
Texture2D ParallaxMap : register(t5);
Texture2D TransparencyMap : register(t6);
Texture2D ReflectionMap : register(t7);
Texture2D OcclusionMap : register(t8);

float4 ComputeAlbedoBuffer(float4 diffuse)
{
    return diffuse;
}

float4 ComputeSpecularBuffer(float4 specular)
{
    return specular;
}

float4 ComputeNormalBuffer(float4 normal)
{
    return normal;
}

float4 ComputeEmissiveBuffer(float4 color, float4 emissive)
{
    if(any(emissive != (float4)0))
    {
        return emissive;
    }
    else
    {
        float brightness = dot(color.rgb, float3(0.4126, 0.7152, 0.0722));
        if(brightness > 0.95)
            return float4(color.rgb, 1.0);
        else
            return (float4)0;
    }
}

float2 ComputeVelocityBuffer(float4 position, float4 prevPosition)
{
    float2 a = (position.xy) * 0.5 + 0.5;
    float2 b = (prevPosition.xy) * 0.5 + 0.5;
    float2 oVelocity = (b - a) * 0.5 + 0.5;

    return oVelocity;
}

struct LightingResult
{
    float3 Diffuse;
    float3 Specular;
};

// d : distance from light
float DoAttenuation( LightData light, float d )
{
    return 1.0f / ( light.AttenuationRadius + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d );
}

// V : view vector
// N : surface normal
float3 DoDiffuse( LightData light, float3 L, float3 N )
{
    float NdotL = max( 0, dot( N, L ) );
    return light.Color * NdotL * light.Intensity;
}

// V : view vector
// L : light vector
// N : surface normal
float3 DoSpecular( LightData light, float3 V, float3 L, float3 N )
{
    // Phong lighting.
    float3 R = normalize( reflect( -L, N ) );
    float RdotV = max( 0, dot( R, V ) );

    // Blinn-Phong lighting
    float3 H = normalize( L + V );
    float NdotH = max( 0, dot( N, H ) );

    return light.Color * pow( RdotV, gSpecularPower ) * light.Intensity;
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoPointLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result;

    float3 L = ( light.Position - P );
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation( light, distance );
 
    result.Diffuse = DoDiffuse( light, L, N ) * attenuation;
    result.Specular = DoSpecular( light, V, L, N ) * attenuation;

    return result;
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoDirectionalLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result;

    float3 L = -light.Direction.xyz;
 
    result.Diffuse = DoDiffuse( light, L, N );
    result.Specular = DoSpecular( light, V, L, N );

    return result;
}

// L : light vector
float DoSpotCone( LightData light, float3 L )
{
    float minCos = cos( light.SpotAngles.x );
    float maxCos = ( minCos + 1.0f ) / 2.0f;
    float cosAngle = dot( light.Direction.xyz, -L );
    return smoothstep( minCos, maxCos, cosAngle ); 
}

// V : view vector
// P : position vector in worldspace
// N : surface normal
LightingResult DoSpotLight( LightData light, float3 V, float3 P, float3 N )
{
    LightingResult result;

    float3 L = ( light.Position - P );
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation( light, distance );
    float spotIntensity = DoSpotCone( light, L );
 
    result.Diffuse = DoDiffuse( light, L, N ) * attenuation * spotIntensity;
    result.Specular = DoSpecular( light, V, L, N ) * attenuation * spotIntensity;

    return result;
}

// P : position vector in world space
// N : normal
LightingResult ComputeLighting( float3 P, float3 N )
{
    float3 V = normalize( gViewOrigin - P );
    LightingResult totalResult = { {0, 0, 0}, {0, 0, 0} };

    [unroll]
    for( uint i = 0; i < gLightsNumber; ++i )
    {
        LightingResult result = { {0, 0, 0}, {0, 0, 0} };

        if(gLights[i].Type == DIRECTIONAL_LIGHT)
            result = DoDirectionalLight( gLights[i], V, P, N );
        else if(gLights[i].Type == POINT_LIGHT)
            result = DoPointLight( gLights[i], V, P, N );
        else if(gLights[i].Type == SPOT_LIGHT)
            result = DoSpotLight( gLights[i], V, P, N );

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

PS_OUTPUT main( PS_INPUT IN )
{
    PS_OUTPUT OUT;

    OUT.Scene  = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float3x3 TBN = float3x3(IN.Tangent.xyz, IN.BiTangent.xyz, IN.Normal.xyz);

    float3 albedo    = gDiffuse.rgb * gDiffuse.a;
    float3 ambient   = gAmbient.rgb * gAmbient.a;
    float3 diffuse   = gDiffuse.rgb * gDiffuse.a;
    float3 emissive  = gEmissive.rgb * gEmissive.a;
    float3 specular  = gSpecular.rgb * gSpecular.a;
    float3 normal    = IN.Normal;
    float  alpha     = gTransparency;
    float2 texCoords = IN.Texture;

    if(gIndexOfRefraction != 1.0)
        { /* TODO */ }
    if(gReflection != 0.0)
        { /* TODO */ }

    if(gUseTransparencyMap == 1)
        alpha = TransparencyMap.Sample( AnisotropicSampler, IN.Texture ).r;
    if(alpha <= gAlphaThreshold)
        discard;
    if(gUseParallaxMap == 1)
        { /* TODO */ }
    if(gUseReflectionMap == 1)
        { /* TODO */ }
    if(gUseNormalMap == 1)
        normal = DoNormalMapping(TBN, NormalMap, AnisotropicSampler, IN.Texture);
    if(gUseBumpMap == 1)
        normal = DoBumpMapping(TBN, BumpMap, AnisotropicSampler, IN.Texture, 1.0f);
    if(gUseDiffuseMap == 1)
        albedo = DiffuseMap.Sample(AnisotropicSampler, IN.Texture).rgb;
    if(gUseSpecularMap == 1)
        specular.rgb = SpecularMap.Sample(AnisotropicSampler, IN.Texture).xyz;
    if(gUseEmissiveMap == 1)
        emissive = emissive * EmissiveMap.Sample( AnisotropicSampler, IN.Texture ).rgb;
    if(gUseOcclusionMap == 1)
        albedo = albedo * OcclusionMap.Sample(AnisotropicSampler, IN.Texture).rgb;

    LightingResult lit = ComputeLighting( IN.WorldPosition.xyz, normalize(normal) );

    diffuse = diffuse * lit.Diffuse.rgb;
    specular = specular * lit.Specular.rgb;

    OUT.Scene.rgb = (ambient + emissive + diffuse + specular) * albedo;
    OUT.Scene.a = alpha;
    
    float4 currentNDC = float4(IN.Position.xyz, 1);
    float4 prevClip = mul(currentNDC, gNDCToPrevNDC);
    float3 prevNdcPos = prevClip.xyz / prevClip.w;

    //OUT.Albedo = ComputeAlbedoBuffer(float4(albedo, 1.0f));
    //OUT.Specular = ComputeSpecularBuffer(float4(specular, 1.0f));
    OUT.Normal = ComputeNormalBuffer(float4(normal, 0.0f));
    OUT.Emissive = ComputeEmissiveBuffer(OUT.Scene, float4(emissive, 0.0));
    OUT.Velocity = ComputeVelocityBuffer(currentNDC, float4(prevNdcPos, 1));

    return OUT;
}
