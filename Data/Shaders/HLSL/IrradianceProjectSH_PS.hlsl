#include "Include/PostProcess.hlsli"
#include "Include/CommonReflectionCubemap.hlsli"
#include "Include/CommonSH.hlsli"

cbuffer PerFrameBuffer : register(b0)
{
    uint gCubeFace;
    float3 gPadding;
}

Texture2D SourceMap : register(t0);

float EvaluateLambert(SHVector coeffs)
{
    // Multiply irradiance SH coefficients by cosine lobe (Lambert diffuse) and evaluate resulting SH
    // See: http://cseweb.ucsd.edu/~ravir/papers/invlamb/josa.pdf for derivation of the
    // cosine lobe factors
    float output = 0.0f;

    // Band 0 (factor 1.0)
    output += coeffs.v[0];

    // Band 1 (factor 2/3)
    float f = (2.0f/3.0f);
    for(uint i = 1; i < 4; i++)
        output += coeffs.v[i] * f;

    // Band 2 (factor 1/4)
    f = (1.0f/4.0f);
    for(i = 4; i < 9; i++)
        output += coeffs.v[i] * f;

    // Band 3 (factor 0)

    // Band 4 (factor -1/24)
    f = (-1.0f/24.0f);
    for(i = 16; i < 25; i++)
        output += coeffs.v[i] * f;

    return output;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 scaledUV = IN.Texture * 2.0f - 1.0f;
    float3 dir = GetDirFromCubeFace(gCubeFace, scaledUV);
    dir = normalize(dir);

    SHVector shBasis = SHBasis(dir);

    SHVectorRGB coeffs = SHLoad(SourceMap, int2(0, 0));
    SHMultiply(coeffs.R, shBasis);
    SHMultiply(coeffs.G, shBasis);
    SHMultiply(coeffs.B, shBasis);

    float3 output = 0;
    output.r = EvaluateLambert(coeffs.R);
    output.g = EvaluateLambert(coeffs.G);
    output.b = EvaluateLambert(coeffs.B);

    return float4(output.rgb, 1.0f);
}
