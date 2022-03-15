#include "Include/ReflectionCubemapCommon.hlsli"
#include "Include/SHCommon.hlsli"

#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define PIXELS_PER_THREAD 4

struct SHCoeffsAndWeight
{
    SHVectorRGB Coeffs;
    float Weight;
};

SamplerState BilinearSampler : register(s0);
TextureCube SourceMap : register(t0);
RWStructuredBuffer<SHCoeffsAndWeight> Output;

cbuffer PerFrameBuffer : register(b0)
{
    uint gCubeFace;
    uint gFaceSize;
    uint2 gDispatchSize;
}

groupshared SHCoeffsAndWeight sCoeffs[TILE_WIDTH * TILE_HEIGHT];

[numthreads(TILE_WIDTH, TILE_HEIGHT, 1)]
void main(
    uint groupIdx : SV_GroupIndex,
    uint3 groupId : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID)
{
    SHCoeffsAndWeight data;
    data.Weight = 0;

    SHZero(data.Coeffs.R);
    SHZero(data.Coeffs.G);
    SHZero(data.Coeffs.B);

    float invFaceSize = 1.0f / gFaceSize;

    uint2 pixelCoords = dispatchThreadId.xy * PIXELS_PER_THREAD;
    uint2 pixelCoordsEnd = pixelCoords + uint2(PIXELS_PER_THREAD, PIXELS_PER_THREAD);
    for(uint y = pixelCoords.y; y < pixelCoordsEnd.y; y++)
    {
        for(uint x = pixelCoords.x; x < pixelCoordsEnd.x; x++)
        {
            // Ignore pixels out of valid range
            if (x >= gFaceSize || y >= gFaceSize)
                break;

            // Map from [0, size-1] to [-1.0 + invSize, 1.0 - invSize].
            // (+0.5 in order to sample center of texel)
            float u = 2.0f * (x + 0.5f) * invFaceSize - 1.0f;
            float v = 2.0f * (y + 0.5f) * invFaceSize - 1.0f;

            float3 dir = GetDirFromCubeFace(gCubeFace, float2(u, v));
            dir = normalize(dir);

            // Need to calculate solid angle (weight) of the texel, as cube face corners have
            // much smaller solid angle, meaning many of them occupy the same area when projected
            // on a sphere. Without weighing that area would look too bright.
            float weight = TexelSolidAngle(u, v, invFaceSize);

            SHVector shBasis = SHBasis(dir);
            float3 radiance = SourceMap.SampleLevel(BilinearSampler, dir, 0).rgb;

            SHMultiplyAdd(data.Coeffs.R, shBasis, radiance.r * weight);
            SHMultiplyAdd(data.Coeffs.G, shBasis, radiance.g * weight);
            SHMultiplyAdd(data.Coeffs.B, shBasis, radiance.b * weight);

            data.Weight += weight;
        }
    }

    sCoeffs[groupIdx] = data;

    GroupMemoryBarrierWithGroupSync();

    uint numThreads = TILE_WIDTH * TILE_HEIGHT;
    [unroll]
    for(uint tc = numThreads / 2; tc > 0; tc >>= 1)
    {
        if(groupIdx < tc)
        {
            SHAdd(sCoeffs[groupIdx].Coeffs.R, sCoeffs[groupIdx + tc].Coeffs.R);
            SHAdd(sCoeffs[groupIdx].Coeffs.G, sCoeffs[groupIdx + tc].Coeffs.G);
            SHAdd(sCoeffs[groupIdx].Coeffs.B, sCoeffs[groupIdx + tc].Coeffs.B);

            sCoeffs[groupIdx].Weight += sCoeffs[groupIdx + tc].Weight;
        }

        GroupMemoryBarrierWithGroupSync();
    }

    if(groupIdx == 0)
    {
        uint faceOffset = gDispatchSize.x * gDispatchSize.y * gCubeFace;
        uint outputIdx = faceOffset + groupId.y * gDispatchSize.x + groupId.x;
        Output[outputIdx] = sCoeffs[0];
    }
}
