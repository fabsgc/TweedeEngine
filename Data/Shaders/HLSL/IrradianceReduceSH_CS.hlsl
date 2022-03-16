#include "Include/CommonReflectionCubemap.hlsli"
#include "Include/CommonSH.hlsli"

#define PI 3.1415926

struct SHCoeffsAndWeight
{
    SHVectorRGB Coeffs;
    float Weight;
};

StructuredBuffer<SHCoeffsAndWeight> Input : register(t0);
RWTexture2D<float4> Output : register(u0);

cbuffer PerFrameBuffer : register(b0)
{
    uint2 gOutputIdx;
    uint gNumEntries;
    uint gPadding;
}

[numthreads(1, 1, 1)]
void main(
    uint groupIdx : SV_GroupIndex,
    uint groupId : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    SHVectorRGB coeffs;
    float weight = 0;

    SHZero(coeffs.R);
    SHZero(coeffs.G);
    SHZero(coeffs.B);

    // Note: There shouldn't be many entries, so we add them all in one thread. Otherwise we should do parallel reduction.
    for(uint i = 0; i < gNumEntries; i++)
    {
        SHCoeffsAndWeight current = Input[i];

        SHAdd(coeffs.R, current.Coeffs.R);
        SHAdd(coeffs.G, current.Coeffs.G);
        SHAdd(coeffs.B, current.Coeffs.B);

        weight += current.Weight;
    }

    // Normalize
    float normFactor = (4 * PI) / weight;
    SHMultiply(coeffs.R, normFactor);
    SHMultiply(coeffs.G, normFactor);
    SHMultiply(coeffs.B, normFactor);

    uint2 writeIdx = gOutputIdx;
    [unroll]
    for(i = 0; i < SH_NUM_COEFFS; ++i)
    {
        Output[writeIdx] = float4(coeffs.R.v[i], coeffs.G.v[i], coeffs.B.v[i], 0.0f);
        writeIdx.x += 1;
    }
}
