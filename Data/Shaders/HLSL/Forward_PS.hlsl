#include "Include/BRDF.hlsli"
#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    PixelData pixelData = (PixelData)0;
    LightingResult lit = (LightingResult)0;

    // #########################################################################
    bool		writeVelocity				= (bool)IN.Other.x;
    bool		castLight					= (bool)IN.Other.y;
    bool		useSRGB						= (bool)IN.Other.z;

    float3		baseColor					= GetColor(useSRGB, true, gMaterial.BaseColor.rgb);
    float3		emissive					= GetColor(useSRGB, true, gMaterial.Emissive.rgb);
    float3		sheenColor					= GetColor(useSRGB, true, gMaterial.SheenColor.rgb);
    float		metallic					= gMaterial.Metallic;
    float		pRoughness					= gMaterial.Roughness;
    float		roughness					= pRoughness * pRoughness;
    float		reflectance					= gMaterial.Reflectance;
    float		occlusion					= gMaterial.Occlusion;
    float		pSheenRoughness				= gMaterial.SheenRoughness;
    float		sheenRoughness				= pSheenRoughness * pSheenRoughness;
    float		clearCoat					= gMaterial.ClearCoat;
    float		pClearCoatRoughness			= gMaterial.ClearCoatRoughness;
    float		clearCoatRoughness			= pClearCoatRoughness * pClearCoatRoughness;
    float		anisotropy					= gMaterial.Anisotropy;
    float3		anisotropyDirection			= gMaterial.AnisotropyDirection;
    float		alphaThreshold				= gMaterial.AlphaTreshold;
    float		parallaxScale				= gMaterial.ParallaxScale;
    uint		parallaxSamples				= gMaterial.ParallaxSamples;
    float		microThickness				= gMaterial.MicroThickness;
    float		thickness					= gMaterial.Thickness;
    float		transmission				= gMaterial.Transmission;
    float3		absorption					= gMaterial.Absorption;

    bool		useBaseColorMap				= (bool)gMaterial.UseBaseColorMap;
    bool		useMetallicMap				= (bool)gMaterial.UseMetallicMap;
    bool		useRoughnessMap				= (bool)gMaterial.UseRoughnessMap;
    bool		useReflectanceMap			= (bool)gMaterial.UseReflectanceMap;
    bool		useOcclusionMap				= (bool)gMaterial.UseOcclusionMap;
    bool		useEmissiveMap				= (bool)gMaterial.UseEmissiveMap;
    bool		useSheenColorMap			= (bool)gMaterial.UseSheenColorMap;
    bool		useSheenRoughnessMap		= (bool)gMaterial.UseSheenRoughnessMap;
    bool		useClearCoatMap				= (bool)gMaterial.UseClearCoatMap;
    bool		useClearCoatRoughnessMap	= (bool)gMaterial.UseClearCoatRoughnessMap;
    bool		useClearCoatNormalMap		= (bool)gMaterial.UseClearCoatNormalMap;
    bool		useNormalMap				= (bool)gMaterial.UseNormalMap;
    bool		useParallaxMap				= (bool)gMaterial.UseParallaxMap;
    bool		useTransmissionMap			= (bool)gMaterial.UseTransmissionMap;
    bool		useRadianceMap				= (bool)gMaterial.UseRadianceMap;
    bool		useDiffuseIrrMap			= (bool)gMaterial.UseDiffuseIrrMap;
    bool		useSpecularIrrMap			= (bool)gMaterial.UseSpecularIrrMap;

    float3		sceneLightColor				= gSceneLightColor.rgb;

    float3		N							= IN.Normal;
    float3		N_clearCoat					= IN.Normal;
    float3		P							= IN.PositionWS.xyz;
    float3		V							= IN.ViewDirWS;
    float3		Pv							= gCamera.ViewOrigin;

    float3x3	TBN							= float3x3(IN.Tangent, IN.BiTangent, N);
    float2		uv0							= (IN.UV0 * gMaterial.UV0Repeat) + gMaterial.UV0Offset;
    float2		uv1							= IN.UV1;

    float3		NDCPos						= (IN.CurrPosition / IN.CurrPosition.w).xyz;
    float3		PrevNDCPos					= (IN.PrevPosition / IN.PrevPosition.w).xyz;

    float3		diffuseBaseColor			= (1.0 - metallic) * baseColor.rgb;
    float3		diffuseSheenColor			= (1.0 - metallic) * sheenColor.rgb;
    // #########################################################################

    // ###################### PARALLAX MAPPING
    if(useParallaxMap)
        uv0 = DoParallaxMapping(uv0, V, Pv, N, P, 
            parallaxSamples, parallaxScale, IN.ParallaxOffsetTS);

    // ###################### TRANSMISSION SAMPLE
    if(useTransmissionMap == 1)
        transmission = TransmissionMap.Sample(TextureSampler, uv0).r;

    // ###################### DISCARD ALPHA THRESHOLD
    if(transmission < alphaThreshold)
    {
        OUT.Scene = (float4)0;
        OUT.Normal = (float4)0;
        OUT.Emissive = (float4)0;
        OUT.Velocity = (float4)0;
    }
    else
    {
        // ###################### NORMAL MAP SAMPLING
        if(useNormalMap)
            N = DoNormalMapping(TBN, NormalMap, TextureSampler, uv0);

        // ###################### METALLIC MAP SAMPLING
        if(useMetallicMap)
            metallic = MetallicMap.Sample(TextureSampler, uv0).r;

        // ###################### BASE COLOR MAP SAMPLING
        if(useBaseColorMap)
        {
            baseColor = BaseColorMap.Sample(TextureSampler, uv0).rgb;
            diffuseBaseColor = (1.0 - metallic) * baseColor.rgb;
        }

        // ###################### ROUGHNESS MAP SAMPLING
        if(useRoughnessMap)
        {
            pRoughness = RoughnessMap.Sample(TextureSampler, uv0).r;
            roughness = pRoughness * pRoughness;
        }

        // ###################### CLEAR COAT NORMAL MAP SAMPLING
        if(useClearCoatNormalMap)
            N_clearCoat = DoNormalMapping(TBN, ClearCoatNormalMap, TextureSampler, uv0);

        // ###################### FILL PIXEL PARAM
        pixelData.DiffuseColor = diffuseBaseColor;
        pixelData.PRoughness = pRoughness;
        pixelData.Roughness = roughness;
        pixelData.F0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
        pixelData.F90 = float3(1.0, 1.0, 1.0);

        // ###################### DO LIGHTING
        lit = DoLighting(V, P, N, pixelData, uv0, castLight, useOcclusionMap);

        lit.Diffuse *= diffuseBaseColor * sceneLightColor + emissive; // TODO PBR
        lit.Specular = lit.Specular;

        OUT.Scene.rgb = lit.Diffuse + lit.Specular;
        OUT.Scene.a = 1.0;

        OUT.Normal = ComputeNormalBuffer(float4(N, 0.0f));
        OUT.Emissive = ComputeEmissiveBuffer(OUT.Scene, float4(emissive, 1.0));

        if(writeVelocity)
            OUT.Velocity = ComputeVelocityBuffer(float4(NDCPos, 0.0), float4(PrevNDCPos, 0.0), transmission);
        else
            OUT.Velocity = float4(0.0, 0.0, 0.0, 1.0);
    }

    return OUT;
}
