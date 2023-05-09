#ifndef __SHADOW__
#define __SHADOW__

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

#endif // __SHADOW__
