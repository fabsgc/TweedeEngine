#include "Include/BRDF.hlsli"
#include "Include/Forward.hlsli"
#include "Include/Forward_PS.hlsli"
#include "Include/CommonMaterial.hlsli"

[earlydepthstencil]
PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    PixelData pixel = (PixelData)0;
    LightingResult lit = (LightingResult)0;

    // #########################################################################
    bool		writeVelocity				= (bool)IN.Other.x;
    bool		castLight					= (bool)IN.Other.y;
    bool		useSRGB						= (bool)IN.Other.z;

    float3		baseColor					= GetColor(useSRGB, true, gMaterial.BaseColor.rgb);
    float3		emissive					= GetColor(useSRGB, true, gMaterial.Emissive.rgb);
    float3		sheenColor					= GetColor(useSRGB, true, gMaterial.SheenColor.rgb);
    float3		subsurfaceColor				= GetColor(useSRGB, true, gMaterial.SubsurfaceColor.rgb);
    float		metallic					= gMaterial.Metallic;
    float		pRoughness					= min(MAX_ROUGHNESS, gMaterial.Roughness);
    float		roughness					= pRoughness * pRoughness;
    float		reflectance					= gMaterial.Reflectance;
    float		occlusion					= gMaterial.Occlusion;
    float		pSheenRoughness				= min(MAX_ROUGHNESS, gMaterial.SheenRoughness);
    float		sheenRoughness				= pSheenRoughness * pSheenRoughness;
    float		clearCoat					= gMaterial.ClearCoat;
    float		pClearCoatRoughness			= min(MAX_ROUGHNESS, gMaterial.ClearCoatRoughness);
    float		subsurfacePower				= gMaterial.SubsurfacePower;
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
    uint		refractionType				= gMaterial.RefractionType;

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
    bool		useAnisotropyDirectionMap	= (bool)gMaterial.UseAnisotropyDirectionMap;

    float3		sceneLightColor				= gSceneLightColor.rgb;

    float3		N							= normalize(IN.Normal);
    float3		N_Raw						= N;
    float3		N_clearCoat					= N;
    float3		N_clearCoat_Raw				= N;
    float3		P							= IN.PositionWS.xyz;
    float3		Pv							= gCamera.ViewOrigin;
    float3		V							= normalize(P - Pv);

    float3x3	TBN							= float3x3(normalize(IN.Tangent), normalize(IN.BiTangent), N);
    float2		uv0							= (IN.UV0 * gMaterial.UV0Repeat) + gMaterial.UV0Offset;
    float2		uv1							= IN.UV1;

    float3		NDCPos						= (IN.CurrPosition / IN.CurrPosition.w).xyz;
    float3		PrevNDCPos					= (IN.PrevPosition / IN.PrevPosition.w).xyz;

    float3		diffuseBaseColor			= (1.0 - metallic) * baseColor.rgb;
    // #########################################################################

    // ###################### PARALLAX MAP MAPPING
    if(useParallaxMap)
        uv0 = DoParallaxMapping(uv0, V, Pv, N, P, 
            parallaxSamples, parallaxScale, IN.ParallaxOffsetTS);

    // ###################### METALLIC MAP SAMPLING
    if(useMetallicMap)
        metallic = MetallicMap.Sample(AnisotropicSampler, uv0).g;

    // ###################### BASE COLOR MAP SAMPLING
    if(useBaseColorMap)
    {
        float4 baseColorFour = BaseColorMap.Sample(AnisotropicSampler, uv0);
        baseColor = baseColorFour.rgb;
        diffuseBaseColor = (1.0 - metallic) * baseColor.rgb;
        transmission = 1 - baseColorFour.a;
    }

    // ###################### TRANSMISSION MAP SAMPLING
    if(useTransmissionMap == 1)
        transmission = TransmissionMap.Sample(AnisotropicSampler, uv0).r;

    // ###################### DISCARD ALPHA THRESHOLD
    if((1.0 - transmission) < alphaThreshold)
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
            N = DoNormalMapping(TBN, NormalMap, AnisotropicSampler, uv0);

        // ###################### ROUGHNESS MAP SAMPLING
        if(useRoughnessMap)
        {
            pRoughness = min(MAX_ROUGHNESS, RoughnessMap.Sample(AnisotropicSampler, uv0).b);
            roughness = pRoughness * pRoughness;
        }

        // ###################### REFLECTANCE MAP SAMPLING
        if(useReflectanceMap)
            reflectance = ReflectanceMap.Sample(AnisotropicSampler, uv0).r;

        // ###################### EMISSIVE MAP SAMPLING
        if(useEmissiveMap)
            emissive *= EmissiveMap.Sample(AnisotropicSampler, uv0).rgb;

        // ###################### SHEEN COLOR MAP SAMPLING
        if(useSheenColorMap)
            sheenColor = SheenColorMap.Sample(AnisotropicSampler, uv0).rgb;

        // ###################### CLEAR COAT ROUGHNESS MAP SAMPLING
        if(useSheenRoughnessMap)
        {
            pSheenRoughness = min(MAX_ROUGHNESS, SheenRoughnessMap.Sample(AnisotropicSampler, uv0).r);
            sheenRoughness = pSheenRoughness * pSheenRoughness;
        }

        // ###################### CLEAR COAT MAP SAMPLING
        if(useClearCoatMap)
            clearCoat = ClearCoatMap.Sample(AnisotropicSampler, uv0).r;

        // ###################### CLEAR COAT ROUGHNESS SAMPLING
        if(useClearCoatRoughnessMap)
        {
            pClearCoatRoughness = min(MAX_ROUGHNESS, ClearCoatRoughnessMap.Sample(AnisotropicSampler, uv0).r);
            clearCoatRoughness = pClearCoatRoughness * pClearCoatRoughness;
        }

        // ###################### CLEAR COAT NORMAL MAP SAMPLING
        if(useClearCoatNormalMap)
            N_clearCoat = DoNormalMapping(TBN, ClearCoatNormalMap, AnisotropicSampler, uv0);

        // ###################### ANISOTROPY DIRECTION MAP SAMPLING
        if(useAnisotropyDirectionMap)
            anisotropyDirection = AnisotropyDirectionMap.Sample(AnisotropicSampler, uv0).rgb;

        if(useOcclusionMap)
            occlusion = OcclusionMap.Sample(AnisotropicSampler, uv0).r;

        // ###################### FILL PIXEL PARAM
        float NoV = ClampNoV(dot(N, V));
        float airIor = 1.0;

        pixel.DiffuseColor = diffuseBaseColor;
        pixel.PRoughness = max(MIN_ROUGHNESS, pRoughness);
        pixel.Roughness = max(MIN_ROUGHNESS, roughness);
        pixel.F0 = ComputeF0(baseColor, metallic, ComputeDielectricF0(reflectance));
        pixel.F90 = saturate(dot(pixel.F0, float3_splat(50.0 * 0.33)));
        pixel.TBN = TBN;
        pixel.Transmission = saturate(transmission);
        pixel.IOR = F0ToIor(pixel.F0.g);
        pixel.EtaIR = airIor / pixel.IOR;  // air -> material
        pixel.EtaRI = pixel.IOR / airIor;  // material -> air
        pixel.RefractionType = refractionType;
        pixel.DFG_GXX = PreIntEnvGF_GXX(NoV, pixel.PRoughness).xyz;

        if(thickness != 0.0 || microThickness != 0.0)
            pixel.Absorption = max((float3)0, absorption);
        else
            pixel.Absorption = saturate(absorption);

        if(thickness != 0.0)
            pixel.Thickness = max(0.0, thickness);

        if(microThickness != 0.0)
            pixel.UThickness = max(0.0, microThickness);

        pixel.ClearCoat = clearCoat;
        pixel.ClearCoatRoughness = max(MIN_ROUGHNESS, clearCoatRoughness);
        pixel.PClearCoatRoughness = max(MIN_ROUGHNESS, pClearCoatRoughness);
        pixel.N_clearCoat = N_clearCoat;

        pixel.SheenColor = sheenColor;
        pixel.SheenRoughness = max(MIN_ROUGHNESS, sheenRoughness);
        pixel.PSheenRoughness = max(MIN_ROUGHNESS, pSheenRoughness);
        pixel.DFG_Charlie = PreIntEnvGF_Charlie(NoV, pixel.PSheenRoughness).xyz;
        pixel.SheenScaling = 1.0 - max(pixel.SheenColor.r, max(pixel.SheenColor.g, pixel.SheenColor.b)) * pixel.DFG_Charlie.z;

        pixel.SubsurfaceColor = subsurfaceColor;
        pixel.SubsurfacePower = subsurfacePower;

        pixel.Anisotropy = anisotropy;
        pixel.AnisotropyDirection = anisotropyDirection;
        pixel.AnisotropicT = normalize(mul(TBN, anisotropyDirection));
        pixel.AnisotropicB = normalize(cross(N_Raw, pixel.AnisotropicT));

        // Energy compensation for multiple scattering in a microfacet model
        pixel.EnergyCompensation = 1.0 + pixel.F0 * (1.0 / pixel.DFG_GXX.y - 1.0);

        // ###################### DO LIGHTING
        lit = DoLighting(V, P, N, pixel, uv0, castLight, occlusion);

        lit.Diffuse *= sceneLightColor + emissive; // TODO PBR
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
