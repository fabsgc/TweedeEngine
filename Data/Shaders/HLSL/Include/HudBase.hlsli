#define STANDARD_MAX_INSTANCED_BLOCK 32

#define HUD_CAMERA 0.0
#define HUD_RADIAL_LIGHT 1.0
#define HUD_DIRECTIONAL_LIGHT 2.0
#define HUD_SPOT_LIGHT 3.0

#define HUD_RENDER_TYPE_DRAW 0
#define HUD_RENDER_TYPE_SELECTION 1
#define HUD_RENDER_TYPE_PICKING 2

struct PerInstanceData
{
    matrix MatWorldNoScale;
    float4 Color;
    float  Type;
    float3 Padding;
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
    float2 Texture: TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float2 Texture: TEXCOORD0;
};
