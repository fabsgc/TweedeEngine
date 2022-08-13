#include "Include/PostProcess.hlsli"

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position = float4(IN.ScreenPosition, 1.0);
    OUT.Texture = IN.Texture;
    OUT.ScreenPosition = IN.ScreenPosition.xy;

    return OUT;
}

// TODO
