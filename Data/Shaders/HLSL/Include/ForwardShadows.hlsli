#ifndef __FORWARD_SHADOWS__
#define __FORWARD_SHADOWS__

#define FORWARD_SHADOW_SLOTS 2
#define FORWARD_SHADOW_CASCADES 6
#define FORWARD_SHADOW_SLOPE_BIAS_SCALE 1.75

cbuffer ForwardShadowBuffer : register(b4)
{
    float4x4 gCascadeMatrices[FORWARD_SHADOW_SLOTS * FORWARD_SHADOW_CASCADES];
    float4 gCascadeSplits[FORWARD_SHADOW_SLOTS * FORWARD_SHADOW_CASCADES];
    float4 gDirectionalShadowParams[FORWARD_SHADOW_SLOTS];
    float4x4 gSpotShadowMatrices[FORWARD_SHADOW_SLOTS];
    float4 gSpotShadowAreas[FORWARD_SHADOW_SLOTS];
    float4 gSpotShadowParams[FORWARD_SHADOW_SLOTS];
    float4 gPointShadowParams[FORWARD_SHADOW_SLOTS];
    float4 gPointShadowBias[FORWARD_SHADOW_SLOTS];
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

float SampleDirectionalCascade(uint slot, uint cascade, float3 worldPosition, float3 worldNormal,
    float3 lightDirection)
{
    uint index = slot * FORWARD_SHADOW_CASCADES + cascade;
    float4 clip = mul(gCascadeMatrices[index], float4(worldPosition, 1.0));
    float3 ndc = clip.xyz / clip.w;
    float2 uv = GetShadowUV(ndc.xy);
    float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gCascadeSplits[index].w *
        (1.0 - saturate(dot(normalize(worldNormal), normalize(lightDirection))));
    float depth = max(ndc.z - receiverBias, 0.0);
    float visibility = 1.0;
    if (all(uv >= 0.0) && all(uv < 1.0) && depth >= 0.0 && depth <= 1.0)
    {
        float3 coord = float3(uv, cascade);
        float storedDepth = 1.0;
        if (slot == 0)
            storedDepth = DirectionalShadowMap0.SampleLevel(NoFilterSampler, coord, 0);
        else
            storedDepth = DirectionalShadowMap1.SampleLevel(NoFilterSampler, coord, 0);
        visibility = step(depth, storedDepth);
    }
    return visibility;
}

float SampleDirectionalShadow(uint slot, float3 worldPosition, float3 worldNormal, float3 lightDirection)
{
    float viewDepth = dot(worldPosition - gCamera.ViewOrigin, gCamera.ViewDir);
    uint cascadeCount = (uint)gDirectionalShadowParams[slot].x;
    [loop]
    for (uint cascade = 0; cascade < cascadeCount; ++cascade)
    {
        float3 split = gCascadeSplits[slot * FORWARD_SHADOW_CASCADES + cascade].xyz;
        if (viewDepth >= split.x && viewDepth <= split.y + split.z)
        {
            float visibility = SampleDirectionalCascade(slot, cascade, worldPosition, worldNormal, lightDirection);
            if (viewDepth > split.y && split.z > 0.0 && cascade + 1 < cascadeCount)
            {
                float nextVisibility = SampleDirectionalCascade(slot, cascade + 1, worldPosition, worldNormal, lightDirection);
                visibility = lerp(visibility, nextVisibility, saturate((viewDepth - split.y) / split.z));
            }
            return visibility;
        }
    }
    return 1.0;
}

float SampleSpotShadow(uint slot, float3 worldPosition, float3 worldNormal, float3 lightDirection)
{
    float4 clip = mul(gSpotShadowMatrices[slot], float4(worldPosition, 1.0));
    float visibility = 1.0;
    if (clip.w > 0.0)
    {
        float3 ndc = clip.xyz / clip.w;
        float2 uv = GetShadowUV(ndc.xy);
        float2 shadowParams = gSpotShadowParams[slot].xy;
        float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gSpotShadowParams[slot].z *
            (1.0 - saturate(dot(normalize(worldNormal), normalize(lightDirection))));
        float depth = max(ndc.z * shadowParams.x - receiverBias, 0.0);
        if (all(uv >= 0.0) && all(uv < 1.0) && depth >= 0.0 && depth <= 1.0)
        {
            float4 area = gSpotShadowAreas[slot];
            float2 atlasUV = area.xy + uv * area.zw;
            float storedDepth = 1.0;
            if (slot == 0)
                storedDepth = SpotShadowMap0.SampleLevel(NoFilterSampler, atlasUV, 0);
            else
                storedDepth = SpotShadowMap1.SampleLevel(NoFilterSampler, atlasUV, 0);
            visibility = lerp(1.0, step(depth, storedDepth), shadowParams.y);
        }
    }
    return visibility;
}

float SamplePointShadow(uint slot, float3 lightToReceiver, float3 worldNormal)
{
    float3 absDirection = abs(lightToReceiver);
    float faceDepth = max(absDirection.x, max(absDirection.y, absDirection.z));
    float4 shadowParams = gPointShadowParams[slot];
    float receiverBias = FORWARD_SHADOW_SLOPE_BIAS_SCALE * gPointShadowBias[slot].x *
        (1.0 - saturate(dot(normalize(worldNormal), normalize(-lightToReceiver))));
    float depth = max((faceDepth * shadowParams.x + shadowParams.y) * shadowParams.z - receiverBias, 0.0);
    float visibility = 1.0;
    if (depth > 0.0 && depth <= 1.0)
    {
        float storedDepth = 1.0;
        if (slot == 0)
            storedDepth = PointShadowMap0.SampleLevel(NoFilterSampler, lightToReceiver, 0);
        else
            storedDepth = PointShadowMap1.SampleLevel(NoFilterSampler, lightToReceiver, 0);
        visibility = lerp(1.0, step(depth, storedDepth), shadowParams.w);
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