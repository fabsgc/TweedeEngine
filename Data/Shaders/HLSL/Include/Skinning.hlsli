#ifndef __SKINNING__
#define __SKINNING__

Buffer<float4> BoneMatrices : register(t0);
Buffer<float4> PrevBoneMatrices : register(t1);

float3x4 GetBoneMatrix(uint idx)
{
    float4 row0 = BoneMatrices[idx * 4 + 0];
    float4 row1 = BoneMatrices[idx * 4 + 1];
    float4 row2 = BoneMatrices[idx * 4 + 2];

    return float3x4(row0, row1, row2);
}

float3x4 GetPrevBoneMatrix(uint idx)
{
    float4 row0 = PrevBoneMatrices[idx * 4 + 0];
    float4 row1 = PrevBoneMatrices[idx * 4 + 1];
    float4 row2 = PrevBoneMatrices[idx * 4 + 2];

    return float3x4(row0, row1, row2);
}

float3x4 GetBlendMatrix(float4 blendWeights, uint4 blendIndices)
{
    float3x4 result = (float3x4)0; 

    if(blendIndices.x >= 0)
        result += blendWeights.x * GetBoneMatrix(blendIndices.x);
    if(blendIndices.y >= 0)
        result += blendWeights.y * GetBoneMatrix(blendIndices.y);
    if(blendIndices.z >= 0)
        result += blendWeights.z * GetBoneMatrix(blendIndices.z);
    if(blendIndices.w >= 0)
        result += blendWeights.w * GetBoneMatrix(blendIndices.w);

    return result;
}

float3x4 GetPrevBlendMatrix(float4 blendWeights, uint4 blendIndices)
{
    float3x4 result = (float3x4)0; 

    if(blendIndices.x >= 0)
        result += blendWeights.x * GetPrevBoneMatrix(blendIndices.x);
    if(blendIndices.y >= 0)
        result += blendWeights.y * GetPrevBoneMatrix(blendIndices.y);
    if(blendIndices.z >= 0)
        result += blendWeights.z * GetPrevBoneMatrix(blendIndices.z);
    if(blendIndices.w >= 0)
        result += blendWeights.w * GetPrevBoneMatrix(blendIndices.w);

    return result;
}

#endif // __SKINNING__
