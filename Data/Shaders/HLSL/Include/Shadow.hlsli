#ifndef __SHADOW__
#define __SHADOW__

#include "Include/Skinning.hlsli"

cbuffer PerShadowBuffer : register(b0)
{
    float4x4 gMatViewProj;
    float2 gNDCZToDeviceZ;
    float gDepthBias;
    float gInvDepthRange;
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
#ifdef USES_GS
    float4 WorldPosition : TEXCOORD0;
#endif // USES_GS
#ifdef USES_PS
    float ShadowPosition : TEXCOORD0;
#endif // USES_PS
};

typedef VS_OUTPUT PS_INPUT;

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

#if SKINNED == 1
    if(gHasAnimation)
    {
        float3x4 blendMatrix = (float3x4)0;
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        worldPosition = float4(mul(blendMatrix, worldPosition), 1.0);
    }
#endif // SKINNED

    worldPosition = mul(gMatWorld, worldPosition);

#ifdef USES_GS
    OUT.WorldPosition = worldPosition;
    OUT.Position = worldPosition;
#else // USES_GS

    // Not using a geometry shader, transform to clip space
    float4 clipPos = mul(gMatViewProj, worldPosition);

    // Clamp geometry behind the near plane
#ifdef CLAMP_TO_NEAR_PLANE
    float ndcZ = clipPos.z / clipPos.w;
    float deviceZ = NDCZToDeviceZ(ndcZ);

    #ifdef USES_PS
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
#ifdef USES_PS
    OUT.ShadowPosition = clipPos.z;
#else // Otherwise apply bias immediately
    clipPos.z = max(DeviceZToNDCZ(0), clipPos.z + gDepthBias);
#endif // USES_PS

    OUT.Position = clipPos;

#endif // USES_GS

    return OUT;
}

#endif // __SHADOW__
