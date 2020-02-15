#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128

float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

struct PerInstanceData
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 BiTangent : BINORMAL;
    float2 Texture : TEXCOORD0;
    float4 Color : COLOR0;
    uint   Instanceid : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BINORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection : POSITION1;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BINORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection: POSITION1;
};
