#include "Include/PostProcessBase.hlsli"
#include "Include/FXAA.hlsli"

cbuffer PerFrameBuffer
{
    float2 gInvTexSize;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    return FxaaPixelShader(IN.Texture, gInvTexSize, 0.75f, 0.125f, 0.0625f);
}
