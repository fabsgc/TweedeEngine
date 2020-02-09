#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128

float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

struct MaterialData
{
    float4 gDiffuse;
    float4 gSpecular;
    float4 gEmissive;
    uint   gUseDiffuseMap;
    uint   gUseSpecularMap;
    uint   gUseNormalMap;
    uint   gUseDepthMap;
    float SpecularPower;
};

struct PerInstanceData
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
};
