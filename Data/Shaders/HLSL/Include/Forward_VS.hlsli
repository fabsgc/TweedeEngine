#ifndef __FORWARD_VS__
#define __FORWARD_VS__

// #################### DEFINES

#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128
#define PER_INSTANCE_LIGHTING_FLAG_CAST_LIGHTS 0x1u
#define PER_INSTANCE_LIGHTING_FLAG_RECEIVE_SHADOWS 0x2u

// #################### STRUCTS

struct PerInstanceData
{
    matrix MatWorld;
    matrix MatInvWorld;
    matrix MatWorldNoScale;
    matrix MatInvWorldNoScale;
    matrix MatPrevWorld;
    uint   Layer;
    uint   HasAnimation;
    uint   WriteVelocity;
    uint   LightingFlags;
};

// #################### CONSTANT BUFFERS

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_FORWARD_MAX_INSTANCED_BLOCK];
};

cbuffer PerObjectBuffer : register(b2)
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

// #################### HELPER FUNCTIONS

#endif // __FORWARD_VS__
