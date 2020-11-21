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
    // xy - (Viewport size in pixels / 2) / Target size in pixels
    // zw - (Viewport offset in pixels + (Viewport size in pixels / 2) + Optional pixel center offset) / Target size in pixels
    float4 	 gClipToUVScaleOffset;
    float4 	 gUVToClipScaleOffset;	
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

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    if(IN.Instanceid == 0)
    {
        OUT.Position.xyz = IN.Position;
        OUT.Position.w = 1.0f;
        OUT.Position = mul(gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OUT.Color = IN.Color;
        OUT.Normal = normalize(mul(gMatWorld, float4(IN.Normal, 0.0f))).xyz;

        OUT.WorldPosition.xyz = IN.Position;
        OUT.WorldPosition.w = 1.0f;
        OUT.WorldPosition = mul(gMatWorld, OUT.WorldPosition);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);
    }
    else
    {
        OUT.Position.xyz = IN.Position;
        OUT.Position.w = 1.0f;
        OUT.Position = mul(gInstanceData[IN.Instanceid].gMatWorld, OUT.Position);
        OUT.Position = mul(gMatViewProj, OUT.Position);

        OUT.Color = IN.Color;
        OUT.Normal = normalize(mul(gInstanceData[IN.Instanceid].gMatWorld, float4(IN.Normal, 0.0f))).xyz;

        OUT.WorldPosition.xyz = IN.Position;
        OUT.WorldPosition.w = 1.0f;
        OUT.WorldPosition = mul(gInstanceData[IN.Instanceid].gMatWorld, OUT.WorldPosition);

        OUT.ViewDirection = normalize(OUT.WorldPosition.xyz - gViewOrigin);
    }

    return OUT;
}
