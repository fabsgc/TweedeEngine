#include "Include/ForwardBase.hlsli"
#include "Include/ForwardBasePS.hlsli"

[earlydepthstencil]
PS_OUTPUT main( PS_INPUT IN )
{
    PS_OUTPUT OUT = (PS_OUTPUT)0;
    float alpha = gTransparency;
    bool writeVelocity = (bool)IN.Other.x;
    bool castLight = (bool)IN.Other.y;

    if(gUseTransparencyMap == 1)
        alpha = TransparencyMap.Sample( TextureSampler, IN.Texture ).r;

    clip(alpha - gAlphaThreshold);

    if(alpha < gAlphaThreshold)
    {
        OUT.Scene = (float4)0;
        OUT.Normal = (float4)0;
        OUT.Emissive = (float4)0;
        OUT.Velocity = (float4)0;
    }
    else
    {
        OUT.Scene  = float4(1.0f, 1.0f, 1.0f, 1.0f);

        float3 albedo            = gDiffuse.rgb;
        float3 ambient           = gAmbient.rgb;
        float3 diffuse           = gDiffuse.rgb;
        float3 emissive          = gEmissive.rgb;
        float3 specular          = gSpecular.rgb;
        float3 sceneLightColor   = gSceneLightColor.rgb;
        float3 globalIllum       = float3(1.0f, 1.0f, 1.0f);
        float3 environment       = (float3)0;
        float3 normal            = IN.Normal;
        float2 texCoords         = (IN.Texture * gTextureRepeat) + gTextureOffset;
        float  specularIntensity = 1.0f; // Computed from specular map

        float3x3 TBN = float3x3(IN.Tangent.xyz, IN.BiTangent.xyz, IN.Normal.xyz);

        if(gUseParallaxMap == 1)
        {
            uint parraxMaxSamples = gParallaxSamples;
            if(parraxMaxSamples < 16) parraxMaxSamples = 16;
            if(parraxMaxSamples > PARALLAX_MAX_SAMPLE) parraxMaxSamples = 256;

            // As Parallax Occlusion Mapping has a very big cost
            // We adjust 
            float3 fragToView = ( IN.PositionWS.xyz - gViewOrigin );
            float distance = length(fragToView);

            if(distance < 750.0f)
            {
                if(distance > 450)
                    parraxMaxSamples /= 24;
                else if(distance > 350)
                    parraxMaxSamples /= 16;
                else if(distance > 250)
                    parraxMaxSamples /= 8;
                else if(distance > 175)
                    parraxMaxSamples /= 4;
                else if(distance > 75)
                    parraxMaxSamples /= 2;

                if(parraxMaxSamples < 16) parraxMaxSamples = 16;

                // Utilize dynamic flow control to change the number of samples per ray 
                // depending on the viewing angle for the surface. Oblique angles require 
                // smaller step sizes to achieve more accurate precision for computing displacement.
                // We express the sampling rate as a linear function of the angle between 
                // the geometric normal and the view direction ray:
                int parallaxSteps = (int)lerp( parraxMaxSamples, PARALLAX_MIN_SAMPLE, dot( IN.ViewDirWS, IN.Normal ) );
                texCoords = DoParallaxMapping(texCoords, IN.ParallaxOffsetTS, parallaxSteps, gParallaxScale);
            }
        }

        if(gUseNormalMap == 1)
            normal = DoNormalMapping(TBN, NormalMap, TextureSampler, texCoords);

        if(gUseBumpMap == 1)
            normal = DoBumpMapping(TBN, BumpMap, TextureSampler, texCoords, gBumpScale);

        if(gUseDiffuseMap == 1)
        {
            float4 diffuseColor = DiffuseMap.Sample(TextureSampler, texCoords);
            albedo = diffuseColor.rgb;
            ambient = albedo;

            if(gUseTransparencyMap == 0)
            {
                alpha = diffuseColor.a;
                clip(alpha - gAlphaThreshold);
            }
        }

        if(gUseEnvironmentMap == 1 || gUseSkyboxMap == 1)
        {
            if(gUseReflectionMap == 1)
            { /* TODO */ }

            if(gIndexOfRefraction != 0.0)
                environment = DoRefraction(IN.PositionWS.xyz, normal);
            if(gReflection != 0.0)
                environment = environment + DoReflection(IN.PositionWS.xyz, normal);

            float reflectAndRefract = gReflection + gRefraction;
            if(reflectAndRefract > 1.0) reflectAndRefract = 1.0;
            albedo = albedo * (1.0 - reflectAndRefract);
        }

        if(gUseSpecularMap == 1)
            specularIntensity = SpecularMap.Sample(TextureSampler, texCoords).y; // specularIntensity is store in green channel

        if(gUseEmissiveMap == 1)
            emissive = emissive * EmissiveMap.Sample(TextureSampler, texCoords).rgb;

        LightingResult lit = ComputeLighting(IN.PositionWS.xyz, normalize(normal), castLight);

        if(gUseOcclusionMap == 1)
        {
            float3 occlusion = OcclusionMap.Sample(TextureSampler, texCoords).rgb;
            lit.Diffuse = lit.Diffuse * occlusion;
            sceneLightColor = sceneLightColor * occlusion;
        }

        if(gUseGlobalIllumination == 1 && (gUseIrradianceMap == 1 || gUseSkyboxIrradianceMap == 1))
            globalIllum = DoGlobalIllumination(normal);

        diffuse = diffuse * lit.Diffuse.rgb;
        specular = specularIntensity * specular * lit.Specular.rgb;

        OUT.Scene.rgb = (globalIllum * ambient + emissive + diffuse + specular) * (albedo + environment);
        OUT.Scene.rgb = sceneLightColor * OUT.Scene.rgb;
        OUT.Scene.a = alpha;

        float3 NDCPos = (IN.CurrPosition / IN.CurrPosition.w).xyz;
        float3 PrevNDCPos = (IN.PrevPosition / IN.PrevPosition.w).xyz;

        OUT.Normal = ComputeNormalBuffer(float4(normal, 0.0f));
        OUT.Emissive = ComputeEmissiveBuffer(OUT.Scene, float4(emissive, 1.0));

        if(writeVelocity)
            OUT.Velocity = ComputeVelocityBuffer(float4(NDCPos, 0.0), float4(PrevNDCPos, 0.0), alpha);
        else
            OUT.Velocity = float4(0.0, 0.0, 0.0, 1.0);
    }

    return OUT;
}
