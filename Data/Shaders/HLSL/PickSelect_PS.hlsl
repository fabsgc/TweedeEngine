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
    float4 PositionWS : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR0;
};

float4 main( PS_INPUT IN ) : SV_Target
{
    float4 output = (float4)0;

    if(gRenderType == RENDER_TYPE_SELECTION)
        output = float4(1.0, 0.5, 0.0, 0.05);
    else
        output = IN.Color;

    return output;
}
