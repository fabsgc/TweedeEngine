#define STANDARD_MAX_INSTANCED_BLOCK 256

struct PerInstanceData
{
    float4 FromColor;
    float4 ToColor;
    float3 From;
    float  Padding1;
    float3 To;
    float  Padding2;
};

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

struct GS_INPUT
{
    float4 Position : SV_POSITION;
};

struct GS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};
