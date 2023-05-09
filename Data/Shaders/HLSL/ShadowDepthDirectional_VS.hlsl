#define CLAMP_TO_NEAR_PLANE 1
#define USE_PS 1

#include "Include/Shadow.hlsli"
#include "Include/Skinning.hlsli"

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    float4 worldPosition = float4(IN.Position, 1.0f);

#if SKINNED == 1
    if(gHasAnimation)
    {
        float3x4 blendMatrix = (float3x4)0;
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        worldPosition = float4(mul(blendMatrix, worldPosition), 1.0);
    }
#endif // SKINNED

    worldPosition = mul(gMatWorld, worldPosition);

#ifdef USES_GS
    OUT.WorldPosition = worldPosition;
    OUT.Position = worldPosition;
#else // USES_GS

    // Not using a geometry shader, transform to clip space
    float4 clipPos = mul(gMatViewProj, worldPosition);

    // Clamp geometry behind the near plane
    #ifdef CLAMP_TO_NEAR_PLANE
        float ndcZ = clipPos.z / clipPos.w;
        float deviceZ = NDCZToDeviceZ(ndcZ);

        #ifdef USES_PS
        if (deviceZ < 0)
        #else
        if (deviceZ < -gDepthBias)
        #endif
        {
            clipPos.z = DeviceZToNDCZ(0);
            clipPos.w = 1.0f;
        }
    #endif // CLAMP_TO_NEAR_PLANE

    // If using a pixel shader, output shadow depth in clip space, as
    // we'll apply bias to it in PS (depth needs to be interpolated in
    // a perspective correct way)
#ifdef USES_PS
    OUT.ShadowPosition = clipPos.z;
#else // Otherwise apply bias immediately
    clipPos.z = max(DeviceZToNDCZ(0), clipPos.z + gDepthBias);
#endif // USES_PS

    OUT.Position = clipPos;

#endif // USES_GS

    return OUT;
}

// TODO Shadow
