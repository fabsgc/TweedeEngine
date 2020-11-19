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

float3x4 GetBoneMatrix(uint idx)
{
    float4 row0 = BoneMatrices[idx * 3 + 0];
    float4 row1 = BoneMatrices[idx * 3 + 1];
    float4 row2 = BoneMatrices[idx * 3 + 2];

    return float3x4(row0, row1, row2);
}

float3x4 GetPrevBoneMatrix(uint idx)
{
    float4 row0 = PrevBoneMatrices[idx * 3 + 0];
    float4 row1 = PrevBoneMatrices[idx * 3 + 1];
    float4 row2 = PrevBoneMatrices[idx * 3 + 2];

    return float3x4(row0, row1, row2);
}

float3x4 GetBlendMatrix(float4 blendWeights, float4 blendIndices)
{
    /*float3x4 result = (float3x4)0;

    if(blendIndices.x >= 0.0)
    {
        result += blendWeights.x * GetBoneMatrix((uint)blendIndices.x);
    }

    if(blendIndices.y >= 0.0)
    {
        result += blendWeights.y * GetBoneMatrix((uint)blendIndices.y);
    }

    if(blendIndices.z >= 0.0)
    {
        result += blendWeights.z * GetBoneMatrix((uint)blendIndices.z);
    }

    if(blendIndices.w >= 0.0)
    {
        result += blendWeights.w * GetBoneMatrix((uint)blendIndices.w);
    }*/
    
    float3x4 result = blendWeights.x * GetBoneMatrix((uint)blendIndices.x);
    result += blendWeights.y * GetBoneMatrix((uint)blendIndices.y);
    result += blendWeights.z * GetBoneMatrix((uint)blendIndices.z);
    result += blendWeights.w * GetBoneMatrix((uint)blendIndices.w);

    return result;
}

float3x4 GetPrevBlendMatrix(float4 blendWeights, float4 blendIndices)
{
    float3x4 result = blendWeights.x * GetPrevBoneMatrix((uint)blendIndices.x);
    result += blendWeights.y * GetPrevBoneMatrix((uint)blendIndices.y);
    result += blendWeights.z * GetPrevBoneMatrix((uint)blendIndices.z);
    result += blendWeights.w * GetPrevBoneMatrix((uint)blendIndices.w);

    return result;
}

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    if(IN.Instanceid == 0)
    {
        float3x4 blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);

        //matrix<float,3,4> m1 = blendMatrix;
        //matrix<float,4,3> m2 = transpose(m1);

        //OUT.Position.xyz = BoneMatrices[0].xyz;
        //OUT.Position.xyz += IN.Position;
        //OUT.Position.w = 1.0f;

        OUT.Position = float4(IN.Position.xyz, 1.0f);
        OUT.Position = float4(mul(blendMatrix, OUT.Position), 1.0f);
        OUT.Position = mul(OUT.Position, gMatWorld);
        OUT.Position = mul(OUT.Position, gMatViewProj);

        OUT.CurrPosition.xyz = IN.Position;
        OUT.CurrPosition.w = 1.0f;
        OUT.CurrPosition = mul(OUT.CurrPosition, gMatWorld);
        OUT.CurrPosition = mul(OUT.CurrPosition, gMatViewProj);

        OUT.PrevPosition.xyz = IN.Position;
        OUT.PrevPosition.w = 1.0f;
        OUT.PrevPosition = mul(OUT.PrevPosition, gMatPrevWorld);
        OUT.PrevPosition = mul(OUT.PrevPosition, gMatViewProj);

        OUT.Normal = normalize(mul(float4(IN.Normal, 0.0f), gMatWorld)).xyz;
        OUT.Tangent = normalize(mul(float4(IN.Tangent.xyz, 0.0f), gMatWorld)).xyz;
        OUT.BiTangent = normalize(mul(float4(IN.BiTangent.xyz, 0.0f), gMatWorld)).xyz;
        OUT.Texture = FlipUV(IN.Texture);

        OUT.WorldPosition.xyz = IN.Position;
        OUT.WorldPosition.w = 1.0f;
        OUT.WorldPosition = mul(OUT.WorldPosition, gMatWorld);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);

        OUT.WorldViewDistance = mul(OUT.WorldPosition, gMatView);
    }
    else
    {
        OUT.Position.xyz = IN.Position;
        OUT.Position.w = 1.0f;
        OUT.Position = mul(OUT.Position, gInstanceData[IN.Instanceid].gMatWorld);
        OUT.Position = mul(OUT.Position, gMatViewProj);

        OUT.CurrPosition.xyz = IN.Position;
        OUT.CurrPosition.w = 1.0f;
        OUT.CurrPosition = mul(OUT.CurrPosition, gInstanceData[IN.Instanceid].gMatWorld);
        OUT.CurrPosition = mul(OUT.CurrPosition, gMatViewProj);

        OUT.PrevPosition.xyz = IN.Position;
        OUT.PrevPosition.w = 1.0f;
        OUT.PrevPosition = mul(OUT.PrevPosition, gInstanceData[IN.Instanceid].gMatPrevWorld);
        OUT.PrevPosition = mul(OUT.PrevPosition, gMatViewProj);

        OUT.Normal = normalize(mul(float4(IN.Normal, 0.0f), gInstanceData[IN.Instanceid].gMatWorld)).xyz;
        OUT.Tangent = normalize(mul(float4(IN.Tangent.xyz, 0.0f), gInstanceData[IN.Instanceid].gMatWorld)).xyz;
        OUT.BiTangent = normalize(mul(float4(IN.BiTangent.xyz, 0.0f), gInstanceData[IN.Instanceid].gMatWorld)).xyz;
        OUT.Texture = FlipUV(IN.Texture);

        OUT.WorldPosition.xyz = IN.Position;
        OUT.WorldPosition.w = 1.0f;
        OUT.WorldPosition = mul(OUT.WorldPosition, gInstanceData[IN.Instanceid].gMatWorld);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);

        OUT.WorldViewDistance = mul(OUT.WorldPosition, gMatView);
    }

    return OUT;
}
