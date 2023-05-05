#ifndef __SHADOW__
#define __SHADOW__

cbuffer PerShadowBuffer : register(b0)
{
    float4x4 gMatViewProj;
    float2 gNDCZToDeviceZ;
    float gDepthBias;
    float gInvDepthRange;
};

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
    uint   gHasAnimation;
    uint   gWriteVelocity;
    uint   gCastLights;
    uint   gReceiveShadows;
    float3 gPadding;
}

#endif // __SHADOW__
