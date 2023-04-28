#ifndef __FORWARD_VS__
#define __FORWARD_VS__

// #################### DEFINES

#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128

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
    uint   CastLights;
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
}

// #################### HELPER FUNCTIONS

#endif // __FORWARD_VS__
