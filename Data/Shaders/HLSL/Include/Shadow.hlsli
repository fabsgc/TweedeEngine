#ifndef __SHADOW__
#define __SHADOW__

#include "Include/Skinning.hlsli"

cbuffer PerShadowBuffer : register(b0)
{
    float4x4 gMatViewProj;
    float2 gNDCZToDeviceZ;
    float gDepthBias;
    float gInvDepthRange;
    float4 gShadowLightDirection;
    float4 gShadowLightPosition;
};

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
    uint   gHasAnimation;
    uint   gWriteVelocity;
    uint   gCastLights;
    uint   gReceiveShadows;
    float3 gPadding;
}

struct VS_INPUT
{
    float3 Position      : POSITION;
    uint4  BlendIndices  : BLENDINDICES;
    float4 BlendWeights  : BLENDWEIGHT;
    float3 Normal        : NORMAL;
    float4 Tangent       : TANGENT;
    float4 BiTangent     : BINORMAL;
    float2 UV0           : TEXCOORD0;
    float2 UV1           : TEXCOORD1;
    float4 Color         : COLOR0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
#if USES_GS
    float4 WorldPosition : TEXCOORD0;
#endif // USES_GS
#if USES_PS
    float ShadowPosition : TEXCOORD0;
#endif // USES_PS
#if USE_PARALLAX_MAP == 1
    float2 UV0 : TEXCOORD1;
    float3 WorldPosition : TEXCOORD2;
    float3 WorldNormal : TEXCOORD3;
    float3 WorldTangent : TEXCOORD4;
    float3 WorldBitangent : TEXCOORD5;
#endif // USE_PARALLAX_MAP
};

typedef VS_OUTPUT PS_INPUT;

#if USE_PARALLAX_MAP == 1
cbuffer ShadowParallaxBuffer : register(b4)
{
    float4 gShadowParallaxParams;
    float4 gShadowUV0Transform;
};

Texture2D ParallaxMap : register(t13);
SamplerState AnisotropicSampler : register(s0);
#endif // USE_PARALLAX_MAP

/** Converts Z value from device range ([0, 1]) to NDC space. */
float DeviceZToNDCZ(float deviceZ)
{
    return deviceZ / gNDCZToDeviceZ.x - gNDCZToDeviceZ.y;
}

/** Converts Z value from NDC space to device Z value in range [0, 1]. */
float NDCZToDeviceZ(float ndcZ)
{
    return (ndcZ + gNDCZToDeviceZ.y) * gNDCZToDeviceZ.x;
}

VS_OUTPUT VS_MAIN(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    float4 worldPosition = float4(IN.Position, 1.0f);
#if USE_PARALLAX_MAP == 1
    float3 worldNormal = IN.Normal;
    float3 worldTangent = IN.Tangent.xyz;
    float3 worldBitangent = IN.BiTangent.xyz;
#endif // USE_PARALLAX_MAP

#if SKINNED == 1
    if(gHasAnimation)
    {
        float3x4 blendMatrix = (float3x4)0;
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        worldPosition = float4(mul(blendMatrix, worldPosition), 1.0);
#if USE_PARALLAX_MAP == 1
        worldNormal = mul(blendMatrix, float4(worldNormal, 0.0f)).xyz;
        worldTangent = mul(blendMatrix, float4(worldTangent, 0.0f)).xyz;
        worldBitangent = mul(blendMatrix, float4(worldBitangent, 0.0f)).xyz;
#endif // USE_PARALLAX_MAP
    }
#endif // SKINNED

    worldPosition = mul(gMatWorld, worldPosition);
#if USE_PARALLAX_MAP == 1
    worldNormal = normalize(mul(gMatWorld, float4(worldNormal, 0.0f)).xyz);
    worldTangent = normalize(mul(gMatWorld, float4(worldTangent, 0.0f)).xyz);
    worldBitangent = normalize(mul(gMatWorld, float4(worldBitangent, 0.0f)).xyz);
#endif // USE_PARALLAX_MAP

#if USES_GS
    OUT.WorldPosition = worldPosition;
    OUT.Position = worldPosition;
#else // USES_GS

    // Not using a geometry shader, transform to clip space
    float4 clipPos = mul(gMatViewProj, worldPosition);

    // Clamp geometry behind the near plane
#ifdef CLAMP_TO_NEAR_PLANE
    float ndcZ = clipPos.z / clipPos.w;
    float deviceZ = NDCZToDeviceZ(ndcZ);

    #if USES_PS
    if (deviceZ < 0)
    #else
    if (deviceZ < -gDepthBias)
    #endif
    {
        clipPos.z = DeviceZToNDCZ(0);
        clipPos.w = 1.0f;
    }
#endif // CLAMP_TO_NEAR_PLANE

    // If using a pixel shader, output shadow depth in clip space, as
    // we'll apply bias to it in PS (depth needs to be interpolated in
    // a perspective correct way)
#if USES_PS
    OUT.ShadowPosition = clipPos.z;
#else // Otherwise apply bias immediately
    clipPos.z = max(DeviceZToNDCZ(0), clipPos.z + gDepthBias);
#endif // USES_PS

#if USE_PARALLAX_MAP == 1
    OUT.UV0 = float2(IN.UV0.x - 1.0f, IN.UV0.y);
    OUT.WorldPosition = worldPosition.xyz;
    OUT.WorldNormal = worldNormal;
    OUT.WorldTangent = worldTangent;
    OUT.WorldBitangent = worldBitangent;
#endif // USE_PARALLAX_MAP

    OUT.Position = clipPos;

#endif // USES_GS

    return OUT;
}

#endif // __SHADOW__
