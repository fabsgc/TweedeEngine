#ifndef __SHADOW_PARALLAX_PS__
#define __SHADOW_PARALLAX_PS__

#include "Include/Shadow.hlsli"

#if USE_PARALLAX_MAP == 1
float GetShadowParallaxHeight(float2 uv, float3 viewDirectionTS, float2 uvDx, float2 uvDy)
{
    float parallaxScale = gShadowParallaxParams.x;
    if (abs(parallaxScale) <= 1e-6 || viewDirectionTS.z >= -0.1)
        return 1.0;

    uint numSteps = min(max((uint)gShadowParallaxParams.y / 4, 8u), 32u);
    float stepSize = 1.0 / (float)numSteps;
    float2 uvStep = (viewDirectionTS.xy / viewDirectionTS.z) * parallaxScale * stepSize;
    float currentBound = 1.0;
    float previousHeight = 1.0;
    float2 currentUV = uv;
    float2 point1 = 0.0;
    float2 point2 = 0.0;
    bool foundIntersection = false;

    [loop]
    for (uint step = 0; step < numSteps; step++)
    {
        currentUV -= uvStep;
        float sampledHeight = ParallaxMap.SampleGrad(AnisotropicSampler, currentUV, uvDx, uvDy).r;
        currentBound -= stepSize;

        if (sampledHeight > currentBound)
        {
            point1 = float2(currentBound, sampledHeight);
            point2 = float2(currentBound + stepSize, previousHeight);
            foundIntersection = true;
            break;
        }

        previousHeight = sampledHeight;
    }

    if (!foundIntersection)
        return 1.0;

    float delta2 = point2.x - point2.y;
    float delta1 = point1.x - point1.y;
    float denominator = delta2 - delta1;
    if (abs(denominator) <= 1e-6)
        return saturate(point1.x);

    return saturate((point1.x * delta2 - point2.x * delta1) / denominator);
}
#endif // USE_PARALLAX_MAP

float4 main(PS_INPUT IN, out float OUT : SV_Depth) : SV_Target0
{
    float shadowPosition = IN.ShadowPosition;

#if USE_PARALLAX_MAP == 1
    float2 uv = IN.UV0 * gShadowUV0Transform.xy + gShadowUV0Transform.zw;
    float2 uvDx = ddx(uv);
    float2 uvDy = ddy(uv);

#if SHADOW_DIRECTIONAL == 1
    float3 lightViewDirection = gShadowLightDirection.xyz;
#else
    float3 lightViewDirection = IN.WorldPosition - gShadowLightPosition.xyz;
#endif

    float3x3 TBN = float3x3(
        normalize(IN.WorldTangent),
        normalize(IN.WorldBitangent),
        normalize(IN.WorldNormal));
    float3 normalizedLightViewDirection = normalize(lightViewDirection);
    float3 viewDirectionTS = mul(TBN, normalizedLightViewDirection);
    float parallaxHeight = GetShadowParallaxHeight(uv, viewDirectionTS, uvDx, uvDy);

    float3 worldNormal = normalize(IN.WorldNormal);
    float displacement = gShadowParallaxParams.x * (1.0 - parallaxHeight) /
        max(-dot(worldNormal, normalizedLightViewDirection), 0.1);
    float3 displacedPosition = IN.WorldPosition + normalizedLightViewDirection * displacement;
    float4 displacedClipPosition = mul(gMatViewProj, float4(displacedPosition, 1.0));
    shadowPosition = displacedClipPosition.z;
#endif // USE_PARALLAX_MAP

#if SHADOW_DIRECTIONAL == 1
    OUT = saturate(NDCZToDeviceZ(max(DeviceZToNDCZ(0.0), shadowPosition + gDepthBias)));
#else
    OUT = saturate(shadowPosition * gInvDepthRange + gDepthBias);
#endif
    return 0.0;
}

#endif // __SHADOW_PARALLAX_PS__
