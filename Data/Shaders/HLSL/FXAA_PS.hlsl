#include "Include/FXAA.hlsli"

cbuffer PerFrameBuffer
{
    float2 gInvTexSize;
}

struct PS_INPUT
{
    noperspective float4 Position : SV_POSITION;
    noperspective float2 Texture : TEXCOORD0;
};

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = FxaaPixelShader(IN.Texture, gInvTexSize, 1.0f, 0.125f, 0.0625f);
    output.a = 1.0;

    return output;
}
