// #################### DEFINES

// #################### STRUCTS

struct CameraData
{
    float3 ViewDir;
    uint   ViewportX;
    float3 ViewOrigin;
    uint   ViewportY;
    matrix MatViewProj;
    matrix MatView;
    matrix MatProj;
    matrix MatPrevViewProj;
    matrix NDCToPrevNDC;
    float4 ClipToUVScaleOffset;
    float4 UVToClipScaleOffset;
};

// #################### STAGE INPUTS / OUTPUTS

struct VS_INPUT
{
    float3 Position      : POSITION;
    float3 Normal        : NORMAL;
    float4 Tangent       : TANGENT;
    float4 BiTangent     : BINORMAL;
    float4 BlendWeights  : BLENDWEIGHT;
    uint4  BlendIndices  : BLENDINDICES;
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
    float3 Tangent           : TANGENT;
    float3 BiTangent         : BINORMAL;
    float2 UV0               : TEXCOORD0;
    float2 UV1               : TEXCOORD1;
    float3 ViewDirWS         : TEXCOORD2;
    float3 ViewDirTS         : TEXCOORD3;
    float2 ParallaxOffsetTS  : TEXCOORD4;
    float4 Other             : TEXCOORD5;
    float4 Color             : COLOR0;
};

struct PS_OUTPUT
{
    float4 Scene      : SV_Target0;
    float4 Normal     : SV_Target1;
    float4 Emissive   : SV_Target2;
    float4 Velocity   : SV_Target3;
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
