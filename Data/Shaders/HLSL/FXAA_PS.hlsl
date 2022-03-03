#include "Include/PostProcess.hlsli"
#include "Include/FXAA.hlsli"

cbuffer PerFrameBuffer
{
    float2 gInvTexSize;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 output = FxaaPixelShader(IN.Texture, gInvTexSize, 0.75f, 0.125f, 0.0625f);
    output.a = 1.0;

    return output;
}
