#ifndef __FORWARD__
#define __FORWARD__

#include "Include/Common.hlsli"

// #################### DEFINES

// #################### STRUCTS

// #################### STAGE INPUTS / OUTPUTS

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
    float4 Position          : SV_POSITION;
    float4 PositionWS        : POSITION;
    float4 CurrPosition      : POSITION1;
    float4 PrevPosition      : POSITION2;
    float3 Normal            : NORMAL;
    float4 Tangent           : TANGENT;
    float4 BiTangent         : BINORMAL;
    float2 UV0               : TEXCOORD0;
    float2 UV1               : TEXCOORD1;
    float3 ViewDirWS         : TEXCOORD2;
    float3 ViewDirTS         : TEXCOORD3;
    float2 ParallaxOffsetTS  : TEXCOORD4;
    float4 Other             : TEXCOORD5;
    float4 Color             : COLOR0;
};

struct VS_Z_INPUT
{
    float3 Position      : POSITION;
    uint4  BlendIndices  : BLENDINDICES;
    float4 BlendWeights  : BLENDWEIGHT;
};

struct VS_Z_OUTPUT
{
    float4 Position          : SV_POSITION;
};

struct PS_OUTPUT
{
    float4 Scene      : SV_Target0;
    float4 Normal     : SV_Target1;
    float4 Emissive   : SV_Target2;
    float2 Velocity   : SV_Target3;
};

// #################### CONSTANT BUFFERS

cbuffer PerCameraBuffer : register(b0)
{
    CameraData gCamera;
}

// #################### HELPER FUNCTIONS

float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

#endif // __FOWARD__
