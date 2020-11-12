#define RENDER_TYPE_DRAW 0
#define RENDER_TYPE_SELECTION 1
#define RENDER_TYPE_PICKING 2

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
    uint   gRenderType;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

SamplerState AnisotropicSampler : register(s0);

float4 main( PS_INPUT IN ) : SV_Target
{
    float4 output = (float4)0;

    if(gRenderType == RENDER_TYPE_SELECTION)
        output = float4(0.25, 0.2, 0.5, 1.0);
    else
        output = IN.Color;

    return output;
}
