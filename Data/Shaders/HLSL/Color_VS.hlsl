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
    float4 gClipToUVScaleOffset;
    float4 gUVToClipScaleOffset;
}

cbuffer PerInstanceBuffer : register(b1)
{
    PerInstanceData gInstanceData[STANDARD_FORWARD_MAX_INSTANCED_BLOCK];
}

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

cbuffer PerFrameBuffer : register(b3)
{
    float  gTime;
    float  gFrameDelta;
    uint   gUseSkyboxMap;
    uint   gUseSkyboxIrradianceMap;
    float  gSkyboxBrightness;
    float3 gPadding5;
    float4 gSceneLightColor;
}

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4x4 blendMatrix = (float4x4)0;
    float4x4 prevBlendMatrix = (float4x4)0;

    if(gHasAnimation)
    {
        blendMatrix = GetBlendMatrix(IN.BlendWeights, IN.BlendIndices);
        prevBlendMatrix = GetPrevBlendMatrix(IN.BlendWeights, IN.BlendIndices);
    }

    if(IN.Instanceid == 0)
    {
        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = mul(blendMatrix, OUT.Position);
        OUT.Position = mul(gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

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

        OUT.PositionWS = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PositionWS = mul(blendMatrix, OUT.PositionWS);
        OUT.PositionWS = mul(gMatWorld, OUT.PositionWS);

        OUT.ViewDirectionWS = normalize(OUT.PositionWS.xyz - gViewOrigin);
        OUT.Color = IN.Color;
    }
    else
    {
        OUT.Position = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.Position = mul(blendMatrix, OUT.Position);
        OUT.Position = mul(gInstanceData[IN.Instanceid].gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

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

        OUT.PositionWS = float4(IN.Position, 1.0f);
        if(gHasAnimation)
            OUT.PositionWS = mul(blendMatrix, OUT.PositionWS);
        OUT.PositionWS = mul(gInstanceData[IN.Instanceid].gMatWorld, OUT.PositionWS);

        OUT.ViewDirectionWS = normalize(OUT.PositionWS.xyz - gViewOrigin);
        OUT.Color = IN.Color;
    }

    return OUT;
}
