#include "Include/ForwardBase.hlsli"
#include "Include/Skinning.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float  gPadding1;
    float3 gViewOrigin;
    float  gPadding2;
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
    float  gPadding3;
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
    uint   gWriteVelocity;
    float  gPadding4;
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

//VS_OUTPUT main( VS_INPUT IN, uint instanceid : SV_InstanceID )
VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4x4 blendMatrix = (float4x4)0;
    float4x4 prevBlendMatrix = (float4x4)0;

    uint instanceid = 0;

    if(instanceid == 0)
    {
        if(gHasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        }

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

        OUT.Normal = IN.Normal;
        OUT.Tangent = IN.Tangent.xyz;
        OUT.BiTangent = IN.BiTangent.xyz;

        if(gHasAnimation)
        {
            OUT.Normal = mul(blendMatrix, float4(OUT.Normal, 0.0f)).xyz;
            OUT.Tangent = mul(blendMatrix, float4(OUT.Tangent, 0.0f)).xyz;
            OUT.BiTangent = mul(blendMatrix, float4(OUT.BiTangent, 0.0f)).xyz;
        }

        OUT.Normal = normalize(mul(gMatWorld, float4(OUT.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gMatWorld, float4(OUT.Tangent, 0.0f))).xyz;
        OUT.BiTangent = normalize(mul(gMatWorld, float4(OUT.BiTangent, 0.0f))).xyz;

        OUT.Texture = FlipUV(IN.Texture);

        OUT.PositionWS = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PositionWS = mul(blendMatrix, OUT.PositionWS);
        OUT.PositionWS = mul(gMatWorld, OUT.PositionWS);
    }
    else
    {
        if(gInstanceData[instanceid].gHasAnimation)
        {
            blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
            prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        }

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

        OUT.Normal = IN.Normal;
        OUT.Tangent = IN.Tangent.xyz;
        OUT.BiTangent = IN.BiTangent.xyz;

        if(gHasAnimation)
        {
            OUT.Normal = mul(blendMatrix, float4(OUT.Normal, 0.0f)).xyz;
            OUT.Tangent = mul(blendMatrix, float4(OUT.Tangent, 0.0f)).xyz;
            OUT.BiTangent = mul(blendMatrix, float4(OUT.BiTangent, 0.0f)).xyz;
        }

        OUT.Normal = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(OUT.Normal, 0.0f))).xyz;
        OUT.Tangent = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(OUT.Tangent, 0.0f))).xyz;
        OUT.BiTangent = normalize(mul(gInstanceData[instanceid].gMatWorld, float4(OUT.BiTangent, 0.0f))).xyz;

        OUT.Texture = FlipUV(IN.Texture);

        OUT.PositionWS = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PositionWS = mul(blendMatrix, OUT.PositionWS);
        OUT.PositionWS = mul(gInstanceData[instanceid].gMatWorld, OUT.PositionWS);
    }

    float3x3 TBN = float3x3(OUT.Tangent, OUT.BiTangent, OUT.Normal);
    OUT.ViewDirWS = normalize(OUT.PositionWS.xyz - gViewOrigin);
    OUT.ViewDirTS = mul(TBN, OUT.ViewDirWS);
    OUT.Color = IN.Color;

    // Compute initial parallax displacement direction:
    float2 ParallaxDirection = normalize(  (OUT.ViewDirTS.xy) );

    // The length of this vector determines the furthest amount of displacement:
    float ViewDirLength = length( (OUT.ViewDirTS) );
    float ParallaxLength = sqrt( ViewDirLength * ViewDirLength - (OUT.ViewDirTS.z) * (OUT.ViewDirTS.z) ) / (OUT.ViewDirTS.z);

    // Compute the actual reverse parallax displacement vector:
    OUT.ParallaxOffsetTS = ParallaxDirection * ParallaxLength;

    return OUT;
}
