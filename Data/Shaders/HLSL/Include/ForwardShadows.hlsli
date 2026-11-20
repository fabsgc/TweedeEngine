#ifndef __FORWARD_SHADOWS__
#define __FORWARD_SHADOWS__

#define FORWARD_SHADOW_SLOTS 2
#define FORWARD_SHADOW_CASCADES 6
#define FORWARD_SHADOW_SLOPE_BIAS_SCALE 1.75

cbuffer ForwardShadowBuffer : register(b4)
{
    float4x4 gCascadeMatrices[FORWARD_SHADOW_SLOTS * FORWARD_SHADOW_CASCADES];
    float4 gCascadeSplits[FORWARD_SHADOW_SLOTS * FORWARD_SHADOW_CASCADES];
    float4 gCascadePCSSParams[FORWARD_SHADOW_SLOTS * FORWARD_SHADOW_CASCADES];
    float4 gDirectionalShadowParams[FORWARD_SHADOW_SLOTS];
    float4x4 gSpotShadowMatrices[FORWARD_SHADOW_SLOTS];
    float4 gSpotShadowAreas[FORWARD_SHADOW_SLOTS];
    float4 gSpotShadowParams[FORWARD_SHADOW_SLOTS];
    float4 gPointShadowParams[FORWARD_SHADOW_SLOTS];
    float4 gPointShadowBias[FORWARD_SHADOW_SLOTS];
    float4 gShadowPCSSParams;
};

Texture2DArray<float> DirectionalShadowMap0 : register(t20);
Texture2DArray<float> DirectionalShadowMap1 : register(t21);
TextureCube<float> PointShadowMap0 : register(t22);
TextureCube<float> PointShadowMap1 : register(t23);
Texture2D<float> SpotShadowMap0 : register(t24);
Texture2D<float> SpotShadowMap1 : register(t25);

float2 GetShadowUV(float2 ndc)
{
    return ndc * float2(0.5, -0.5) + 0.5;
}

float2 GetPCSSDiskOffset(uint index, uint sampleCount, float rotation)
{
    const float GoldenAngle = 2.39996323;
    float radius = sqrt(((float)index + 0.5) / (float)sampleCount);
    float angle = (float)index * GoldenAngle + rotation;
    return radius * float2(cos(angle), sin(angle));
}

float GetPCSSRotation(float2 uv)
{
    float noise = frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    return noise * 6.28318531;
}

uint GetPCSSSampleCount()
{
    return (uint)clamp(gShadowPCSSParams.x, 4.0, 20.0);
}

float SampleDirectionalShadowDepth(uint slot, float3 coord)
{
    float depth = 1.0;
    if (slot == 0)
        depth = DirectionalShadowMap0.SampleLevel(NoFilterSampler, coord, 0);
    else
        depth = DirectionalShadowMap1.SampleLevel(NoFilterSampler, coord, 0);

    return depth;
}

float SampleDirectionalCascade(uint slot, uint cascade, float3 worldPosition, float3 worldNormal,
    float3 lightDirection)
{
    float visibility = 1.0;
    uint index = slot * FORWARD_SHADOW_CASCADES + cascade;
    float4 clip = mul(gCascadeMatrices[index], float4(worldPosition, 1.0));
    float3 ndc = clip.xyz / clip.w;
    float2 uv = GetShadowUV(ndc.xy);
    float normalLightDot = saturate(dot(normalize(worldNormal), normalize(lightDirection)));
    float slopeScale = sqrt(max(1.0 - normalLightDot * normalLightDot, 0.0)) /
        max(normalLightDot, 0.1);
    float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gCascadeSplits[index].w *
        (1.0 - normalLightDot);
    float depth = max(ndc.z - receiverBias, 0.0);
    if (all(uv >= 0.0) && all(uv < 1.0) && depth >= 0.0 && depth <= 1.0)
    {
        float sourceAngle = max(gDirectionalShadowParams[slot].y, 0.0);
        float mapSize = max(gDirectionalShadowParams[slot].z, 1.0);
        float storedDepth = SampleDirectionalShadowDepth(slot, float3(uv, cascade));
        if (sourceAngle <= 1e-6)
            visibility = step(depth, storedDepth);
        else
        {
            uint sampleCount = GetPCSSSampleCount();
            float depthRange = max(gCascadePCSSParams[index].x, 1e-4);
            float projectionScale = 0.5 * abs(gCascadeMatrices[index][0][0]);
            float sourceTangent = tan(min(sourceAngle, 0.5));
            float receiverWorldDepth = max(ndc.z, 0.0) * depthRange;
            float texelSize = 1.0 / mapSize;
            float searchRadius = clamp(sourceTangent * receiverWorldDepth * projectionScale,
                texelSize, 0.05);
            uint blockerSamples = max(sampleCount / 2, 4u);
            float searchBias = max(gCascadeSplits[index].w, 0.0) *
                max(searchRadius * mapSize - 3.0, 0.0) * slopeScale / 3.0;
            float rotation = GetPCSSRotation(uv);
            float blockerDepthSum = 0.0;
            uint blockerCount = 0;
            if (storedDepth < depth)
            {
                blockerDepthSum = storedDepth;
                blockerCount = 1;
            }

            [loop]
            for (uint sampleIndex = 0; sampleIndex < blockerSamples; ++sampleIndex)
            {
                float2 sampleUV = uv + GetPCSSDiskOffset(sampleIndex, blockerSamples, rotation) * searchRadius;
                if (all(sampleUV >= 0.0) && all(sampleUV < 1.0))
                {
                    float sampleDepth = SampleDirectionalShadowDepth(slot, float3(sampleUV, cascade));
                    if (sampleDepth < max(depth - searchBias, 0.0))
                    {
                        blockerDepthSum += sampleDepth;
                        blockerCount++;
                    }
                }
            }

            if (blockerCount > 0)
            {
                float averageBlockerDepth = blockerDepthSum / blockerCount;
                float blockerReceiverGap = max(depth - averageBlockerDepth, 0.0) * depthRange;
                float filterRadius = clamp(sourceTangent * blockerReceiverGap * projectionScale,
                    texelSize * 0.5, 0.05);
                float filterBias = max(gCascadeSplits[index].w, 0.0) *
                    max(filterRadius * mapSize - 3.0, 0.0) * slopeScale / 3.0;
                visibility = step(depth, storedDepth);

                [loop]
                for (uint filterIndex = 0; filterIndex < sampleCount; ++filterIndex)
                {
                    float2 sampleUV = uv + GetPCSSDiskOffset(filterIndex, sampleCount, rotation) * filterRadius;
                    float sampleDepth = 1.0;
                    if (all(sampleUV >= 0.0) && all(sampleUV < 1.0))
                        sampleDepth = SampleDirectionalShadowDepth(slot, float3(sampleUV, cascade));

                    visibility += step(max(depth - filterBias, 0.0), sampleDepth);
                }

                visibility /= sampleCount + 1;
            }
        }
    }

    return visibility;
}

float SampleDirectionalShadow(uint slot, float3 worldPosition, float3 worldNormal, float3 lightDirection)
{
    float visibility = 1.0;
    float viewDepth = dot(worldPosition - gCamera.ViewOrigin, gCamera.ViewDir);
    uint cascadeCount = (uint)gDirectionalShadowParams[slot].x;
    [loop]
    for (uint cascade = 0; cascade < cascadeCount; ++cascade)
    {
        float3 split = gCascadeSplits[slot * FORWARD_SHADOW_CASCADES + cascade].xyz;
        if (viewDepth >= split.x && viewDepth <= split.y + split.z)
        {
            visibility = SampleDirectionalCascade(slot, cascade, worldPosition, worldNormal, lightDirection);
            if (viewDepth > split.y && split.z > 0.0 && cascade + 1 < cascadeCount)
            {
                float nextVisibility = SampleDirectionalCascade(slot, cascade + 1, worldPosition, worldNormal, lightDirection);
                visibility = lerp(visibility, nextVisibility, saturate((viewDepth - split.y) / split.z));
            }
            break;
        }
    }
    return visibility;
}

float SampleSpotShadowDepth(uint slot, float2 atlasUV)
{
    float depth = 1.0;
    if (slot == 0)
        depth = SpotShadowMap0.SampleLevel(NoFilterSampler, atlasUV, 0);
    else
        depth = SpotShadowMap1.SampleLevel(NoFilterSampler, atlasUV, 0);

    return depth;
}

float SampleSpotShadow(uint slot, float3 worldPosition, float3 worldNormal, float3 lightDirection)
{
    float visibility = 1.0;
    float4 clip = mul(gSpotShadowMatrices[slot], float4(worldPosition, 1.0));
    if (clip.w > 0.0)
    {
        float3 ndc = clip.xyz / clip.w;
        float2 uv = GetShadowUV(ndc.xy);
        float4 shadowParams = gSpotShadowParams[slot];
        float normalLightDot = saturate(dot(normalize(worldNormal), normalize(lightDirection)));
        float slopeScale = sqrt(max(1.0 - normalLightDot * normalLightDot, 0.0)) /
            max(normalLightDot, 0.1);
        float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gSpotShadowParams[slot].z *
            (1.0 - normalLightDot);
        float depth = max(clip.z * shadowParams.x - receiverBias, 0.0);
        if (all(uv >= 0.0) && all(uv < 1.0) && depth >= 0.0 && depth <= 1.0)
        {
            float4 area = gSpotShadowAreas[slot];
            float2 atlasUV = area.xy + uv * area.zw;
            float storedDepth = SampleSpotShadowDepth(slot, atlasUV);
            float sourceRadius = max(shadowParams.w, 0.0);
            if (sourceRadius <= 1e-6)
                visibility = lerp(1.0, step(depth, storedDepth), shadowParams.y);
            else
            {
                uint atlasWidth, atlasHeight, numMips;
                if (slot == 0)
                    SpotShadowMap0.GetDimensions(0, atlasWidth, atlasHeight, numMips);
                else
                    SpotShadowMap1.GetDimensions(0, atlasWidth, atlasHeight, numMips);

                uint sampleCount = GetPCSSSampleCount();
                uint blockerSamples = max(sampleCount / 2, 4u);
                float2 localTexelSize = 1.0 / max(area.zw * float2(atlasWidth, atlasHeight), float2(1.0, 1.0));
                float minFilterRadius = max(localTexelSize.x, localTexelSize.y) * 0.5;
                float texelScale = 1.0 / max(localTexelSize.x, localTexelSize.y);
                float projectionScale = 0.5 * abs(gSpotShadowMatrices[slot][0][0]);
                float searchRadius = clamp(sourceRadius / max(clip.w, 0.05) * projectionScale,
                    minFilterRadius, 0.05);
                float searchBias = max(shadowParams.z, 0.0) *
                    max(searchRadius * texelScale - 1.0, 0.0) * slopeScale;
                float rotation = GetPCSSRotation(uv);
                float blockerDepthSum = 0.0;
                uint blockerCount = 0;
                if (storedDepth < depth)
                {
                    blockerDepthSum = storedDepth;
                    blockerCount = 1;
                }

                [loop]
                for (uint sampleIndex = 0; sampleIndex < blockerSamples; ++sampleIndex)
                {
                    float2 sampleUV = uv + GetPCSSDiskOffset(sampleIndex, blockerSamples, rotation) * searchRadius;
                    if (all(sampleUV >= 0.0) && all(sampleUV < 1.0))
                    {
                        float sampleDepth = SampleSpotShadowDepth(slot, area.xy + sampleUV * area.zw);
                        if (sampleDepth < max(depth - searchBias, 0.0))
                        {
                            blockerDepthSum += sampleDepth;
                            blockerCount++;
                        }
                    }
                }

                if (blockerCount == 0)
                    visibility = lerp(1.0, step(depth, storedDepth), shadowParams.y);
                else
                {
                    float averageBlockerDepth = blockerDepthSum / blockerCount;
                    float penumbraRatio = saturate((depth - averageBlockerDepth) /
                        max(averageBlockerDepth, 1e-4));
                    float filterRadius = clamp(searchRadius * penumbraRatio, minFilterRadius, 0.05);
                    float filterBias = max(shadowParams.z, 0.0) *
                        max(filterRadius * texelScale - 1.0, 0.0) * slopeScale;
                    visibility = step(depth, storedDepth);

                    [loop]
                    for (uint filterIndex = 0; filterIndex < sampleCount; ++filterIndex)
                    {
                        float2 sampleUV = uv + GetPCSSDiskOffset(filterIndex, sampleCount, rotation) * filterRadius;
                        float sampleDepth = 1.0;
                        if (all(sampleUV >= 0.0) && all(sampleUV < 1.0))
                            sampleDepth = SampleSpotShadowDepth(slot, area.xy + sampleUV * area.zw);

                        visibility += step(max(depth - filterBias, 0.0), sampleDepth);
                    }

                    visibility = lerp(1.0, visibility / (sampleCount + 1), shadowParams.y);
                }
            }
        }
    }

    return visibility;
}

float SamplePointShadowDepth(uint slot, float3 direction)
{
    float depth = 1.0;
    if (slot == 0)
        depth = PointShadowMap0.SampleLevel(NoFilterSampler, direction, 0);
    else
        depth = PointShadowMap1.SampleLevel(NoFilterSampler, direction, 0);

    return depth;
}

float SamplePointShadow(uint slot, float3 lightToReceiver, float3 worldNormal)
{
    float visibility = 1.0;
    float receiverDistance = length(lightToReceiver);
    float3 absDirection = abs(lightToReceiver);
    float faceDepth = max(absDirection.x, max(absDirection.y, absDirection.z));
    float faceScale = faceDepth / max(receiverDistance, 1e-6);
    float4 shadowParams = gPointShadowParams[slot];
    float3 normalizedWorldNormal = normalize(worldNormal);
    float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gPointShadowBias[slot].x *
        (1.0 - saturate(dot(normalizedWorldNormal, normalize(-lightToReceiver))));
    float depth = max((faceDepth * shadowParams.x + shadowParams.y) * shadowParams.z - receiverBias, 0.0);
    if (depth > 0.0 && depth <= 1.0)
    {
        float storedDepth = SamplePointShadowDepth(slot, lightToReceiver);
        float sourceRadius = max(gPointShadowBias[slot].y, 0.0);
        if (sourceRadius <= 1e-6)
            visibility = lerp(1.0, step(depth, storedDepth), shadowParams.w);
        else
        {
            uint sampleCount = GetPCSSSampleCount();
            uint blockerSamples = max(sampleCount / 2, 4u);
            float3 direction = normalize(lightToReceiver);
            float3 helperAxis = abs(direction.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(0.0, 1.0, 0.0);
            float3 tangent = normalize(cross(helperAxis, direction));
            float3 bitangent = cross(direction, tangent);
            float texelAngle = 2.0 / max((float)gPointShadowBias[slot].z, 1.0);
            float searchRadius = clamp(sourceRadius / max(receiverDistance, 0.05), texelAngle * 0.5, 0.25);
            float rotation = GetPCSSRotation(direction.xy);
            float blockerDepthSum = 0.0;
            uint blockerCount = 0;
            if (storedDepth < depth)
            {
                float blockerFaceDepth = (storedDepth / max(shadowParams.z, 1e-6) - shadowParams.y) /
                    max(shadowParams.x, 1e-6);
                blockerDepthSum = blockerFaceDepth / max(faceScale, 1e-6);
                blockerCount = 1;
            }

            [loop]
            for (uint sampleIndex = 0; sampleIndex < blockerSamples; ++sampleIndex)
            {
                float2 diskOffset = GetPCSSDiskOffset(sampleIndex, blockerSamples, rotation);
                float3 sampleDirection = normalize(direction +
                    (tangent * diskOffset.x + bitangent * diskOffset.y) * searchRadius);
                float sampleFaceScale = max(abs(sampleDirection.x),
                    max(abs(sampleDirection.y), abs(sampleDirection.z)));
                float sampleFaceDepth = receiverDistance * sampleFaceScale;
                float sampleReceiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gPointShadowBias[slot].x *
                    (1.0 - saturate(dot(normalizedWorldNormal, -sampleDirection)));
                float sampleReceiverDepth = max(
                    (sampleFaceDepth * shadowParams.x + shadowParams.y) * shadowParams.z - sampleReceiverBias, 0.0);
                float sampleDepth = SamplePointShadowDepth(slot, sampleDirection);
                if (sampleReceiverDepth <= 1.0 && sampleDepth < sampleReceiverDepth)
                {
                    float blockerFaceDepth = (sampleDepth / max(shadowParams.z, 1e-6) - shadowParams.y) /
                        max(shadowParams.x, 1e-6);
                    blockerDepthSum += blockerFaceDepth / max(sampleFaceScale, 1e-6);
                    blockerCount++;
                }
            }

            if (blockerCount == 0)
                visibility = lerp(1.0, step(depth, storedDepth), shadowParams.w);
            else
            {
                float averageBlockerDepth = blockerDepthSum / blockerCount;
                float penumbraRadius = sourceRadius * max(receiverDistance - averageBlockerDepth, 0.0) /
                    max(averageBlockerDepth * receiverDistance, 0.0025);
                float filterRadius = clamp(penumbraRadius, texelAngle * 0.5, 0.25);
                visibility = step(depth, storedDepth);

                [loop]
                for (uint filterIndex = 0; filterIndex < sampleCount; ++filterIndex)
                {
                    float2 diskOffset = GetPCSSDiskOffset(filterIndex, sampleCount, rotation);
                    float3 sampleDirection = normalize(direction +
                        (tangent * diskOffset.x + bitangent * diskOffset.y) * filterRadius);
                    float sampleFaceScale = max(abs(sampleDirection.x),
                        max(abs(sampleDirection.y), abs(sampleDirection.z)));
                    float sampleFaceDepth = receiverDistance * sampleFaceScale;
                    float sampleReceiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gPointShadowBias[slot].x *
                        (1.0 - saturate(dot(normalizedWorldNormal, -sampleDirection)));
                    float sampleReceiverDepth = max(
                        (sampleFaceDepth * shadowParams.x + shadowParams.y) * shadowParams.z - sampleReceiverBias, 0.0);
                    if (sampleReceiverDepth <= 1.0)
                        visibility += step(sampleReceiverDepth, SamplePointShadowDepth(slot, sampleDirection));
                    else
                        visibility += 1.0;
                }

                visibility = lerp(1.0, visibility / (sampleCount + 1), shadowParams.w);
            }
        }
    }

    return visibility;
}

float GetForwardShadowVisibility(LightData light, float3 worldPosition, float3 worldNormal)
{
    float visibility = 1.0;
    if (light.ShadowSlot > 0 && light.ShadowSlot <= FORWARD_SHADOW_SLOTS)
    {
        uint slot = light.ShadowSlot - 1;
        if (light.Type == DIRECTIONAL_LIGHT)
            visibility = SampleDirectionalShadow(slot, worldPosition, worldNormal, -light.Direction);
        else if (light.Type == POINT_LIGHT)
            visibility = SamplePointShadow(slot, worldPosition - light.Position, worldNormal);
        else
            visibility = SampleSpotShadow(slot, worldPosition, worldNormal, light.Position - worldPosition);
    }
    return visibility;
}

#endif