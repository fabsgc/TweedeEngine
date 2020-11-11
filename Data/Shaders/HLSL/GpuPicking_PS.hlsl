#define RENDER_TYPE_SELECTION 0
#define RENDER_TYPE_PICKING 1

cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
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
    {
        output = float4(0.0, 0.0, 1.0, 0.25);
    }
    else
    {
        output = IN.Color;
    }

    return output;
}
