#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128

float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

struct PerInstanceData
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
};
