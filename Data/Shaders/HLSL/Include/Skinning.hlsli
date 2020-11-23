Buffer<float4> BoneMatrices;
Buffer<float4> PrevBoneMatrices;

float4x4 GetBoneMatrix(uint idx)
{
    float4 row0 = BoneMatrices[idx * 4 + 0];
    float4 row1 = BoneMatrices[idx * 4 + 1];
    float4 row2 = BoneMatrices[idx * 4 + 2];
    float4 row3 = BoneMatrices[idx * 4 + 3];

    return float4x4(row0, row1, row2, row3);
}

float4x4 GetPrevBoneMatrix(uint idx)
{
    float4 row0 = PrevBoneMatrices[idx * 4 + 0];
    float4 row1 = PrevBoneMatrices[idx * 4 + 1];
    float4 row2 = PrevBoneMatrices[idx * 4 + 2];
    float4 row3 = PrevBoneMatrices[idx * 4 + 3];

    return float4x4(row0, row1, row2, row3);
}

float4x4 GetBlendMatrix(float4 blendWeights, uint4 blendIndices)
{
    float4x4 result = (float4x4)0; 

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

float4x4 GetPrevBlendMatrix(float4 blendWeights, uint4 blendIndices)
{
    float4x4 result = (float4x4)0; 

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
