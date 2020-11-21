#include "Include/ForwardBase.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
    matrix gMatPrevViewProj;
    matrix gNDCToPrevNDC;
}

cbuffer PerMaterialBuffer : register(b1)
{
    float4 gAmbient;
    float4 gDiffuse;
    float4 gEmissive;
    float4 gSpecular;
    uint   gUseDiffuseMap;
    uint   gUseEmissiveMap;
    uint   gUseNormalMap;
    uint   gUseSpecularMap;
    uint   gUseBumpMap;
    uint   gUseParallaxMap;
    uint   gUseTransparencyMap;
    uint   gUseReflectionMap;
    uint   gUseOcclusionMap;
    uint   gUseEnvironmentMap;
    float  gSpecularPower;
    float  gSpecularStrength;
    float  gTransparency;
    float  gIndexOfRefraction;
    float  gRefraction;
    float  gReflection;
    float  gAbsorbance;
    float  gBumpScale;
    float  gAlphaThreshold;
};

cbuffer PerInstanceBuffer : register(b2)
{
    PerInstanceData gInstanceData[STANDARD_FORWARD_MAX_INSTANCED_BLOCK];
}

cbuffer PerObjectBuffer : register(b3)
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
    uint   gHasAnimation;
}

cbuffer PerFrameBuffer : register(b4)
{
    float gTime;
    float gFrameDelta;
    float4 gSceneLightColor;
}

cbuffer PerCallBuffer : register(b5)
{
    matrix gMatWorldViewProj;
}

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

float4x4 GetBlendMatrix(float4 blendWeights, float4 blendIndices)
{
    float4x4 result = (float4x4)0; 

    if(blendIndices.x >= 0)
        result += blendWeights.x * GetBoneMatrix((uint)blendIndices.x);
    if(blendIndices.y >= 0)
        result += blendWeights.y * GetBoneMatrix((uint)blendIndices.y);
    if(blendIndices.z >= 0)
        result += blendWeights.z * GetBoneMatrix((uint)blendIndices.z);
    if(blendIndices.w >= 0)
        result += blendWeights.w * GetBoneMatrix((uint)blendIndices.w);

    return result;
}

float4x4 GetPrevBlendMatrix(float4 blendWeights, float4 blendIndices)
{
    float4x4 result = (float4x4)0; 

    if(blendIndices.x >= 0)
        result += blendWeights.x * GetPrevBoneMatrix((uint)blendIndices.x);
    if(blendIndices.y >= 0)
        result += blendWeights.y * GetPrevBoneMatrix((uint)blendIndices.y);
    if(blendIndices.z >= 0)
        result += blendWeights.z * GetPrevBoneMatrix((uint)blendIndices.z);
    if(blendIndices.w >= 0)
        result += blendWeights.w * GetPrevBoneMatrix((uint)blendIndices.w);

    return result;
}

VS_OUTPUT main( VS_INPUT IN, uint instanceid : SV_InstanceID )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4x4 blendMatrix = (float4x4)0;
    float4x4 prevBlendMatrix = (float4x4)0;

    if(gHasAnimation)
    {
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
    }

    if(instanceid == 0)
    {
        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = mul(blendMatrix, OUT.Position);
        OUT.Position = mul(gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OUT.CurrPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.CurrPosition = mul(blendMatrix, OUT.CurrPosition);
        OUT.CurrPosition = mul(gMatWorld, OUT.CurrPosition);
        OUT.CurrPosition = mul(gMatViewProj, OUT.CurrPosition);

        OUT.PrevPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PrevPosition = mul(prevBlendMatrix, OUT.PrevPosition);
        OUT.PrevPosition = mul(gMatPrevWorld, OUT.PrevPosition);
        OUT.PrevPosition = mul(gMatPrevViewProj, OUT.PrevPosition);

        OUT.Normal = normalize(mul(gMatWorld, float4(IN.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gMatWorld, float4(IN.Tangent.xyz, 0.0f))).xyz;
        OUT.BiTangent = normalize(mul(gMatWorld, float4(IN.BiTangent.xyz, 0.0f))).xyz;
        OUT.Texture = FlipUV(IN.Texture);

        OUT.WorldPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.WorldPosition = mul(blendMatrix, OUT.WorldPosition);
        OUT.WorldPosition = mul(gMatWorld, OUT.WorldPosition);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);
        OUT.WorldViewDistance = mul(gMatView, OUT.WorldPosition);
    }
    else
    {
        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = mul(blendMatrix, OUT.Position);
        OUT.Position = mul(gInstanceData[instanceid].gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OUT.CurrPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.CurrPosition = mul(blendMatrix, OUT.CurrPosition);
        OUT.CurrPosition = mul(gInstanceData[instanceid].gMatWorld, OUT.CurrPosition);
        OUT.CurrPosition = mul(gMatViewProj, OUT.CurrPosition);

        OUT.PrevPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PrevPosition = mul(prevBlendMatrix, OUT.PrevPosition);
        OUT.PrevPosition = mul(gInstanceData[instanceid].gMatPrevWorld, OUT.PrevPosition);
        OUT.PrevPosition = mul(gMatViewProj, OUT.PrevPosition);

        OUT.Normal = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(IN.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(IN.Tangent.xyz, 0.0f))).xyz;
        OUT.BiTangent = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(IN.BiTangent.xyz, 0.0f))).xyz;
        OUT.Texture = FlipUV(IN.Texture);

        OUT.WorldPosition = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.WorldPosition = mul(blendMatrix, OUT.WorldPosition);
        OUT.WorldPosition = mul(gInstanceData[instanceid].gMatWorld, OUT.WorldPosition);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);
        OUT.WorldViewDistance = mul(OUT.WorldPosition, gMatView);
    }

    return OUT;
}
