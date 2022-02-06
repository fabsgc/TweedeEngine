// #################### DEFINES

// #################### STRUCTS

struct CameraData
{
    float3 gViewDir;
    uint   gViewportX;
    float3 gViewOrigin;
    uint   gViewportY;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
    matrix gMatPrevViewProj;
    matrix gNDCToPrevNDC;
    float4 gClipToUVScaleOffset;
    float4 gUVToClipScaleOffset;
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
    float2 Texture       : TEXCOORD0;
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
    float2 Texture           : TEXCOORD0;
    float3 ViewDirWS         : TEXCOORD1;
    float3 ViewDirTS         : TEXCOORD2;
    float2 ParallaxOffsetTS  : TEXCOORD3;
    float4 Other             : TEXCOORD4;
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
