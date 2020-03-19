#include "Include/PostProcessBase.hlsli"

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

cbuffer PerFrameBuffer : register(b1)
{
    int gHalfNumSamples;
    uint gMSAACount;
}

/** Converts position in NDC to UV coordinates mapped to the screen rectangle. */ 
float2 NDCToUV(float2 ndcPos)
{
    return ndcPos.xy * gClipToUVScaleOffset.xy + gClipToUVScaleOffset.zw;
}

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

Texture2D DepthMap : register(t2);
Texture2DMS<float4> DepthMapMS : register(t3);

float4 main( PS_INPUT IN ) : SV_Target0
{
    int i = 0;
    float2 currentUV = IN.Texture;
    float2 ndcPos = IN.ScreenPosition;

    float curDepth = TextureSampling(BilinearSampler, DepthMap, DepthMapMS, currentUV, gMSAACount).r;

    float4 currentNDC = float4(ndcPos, curDepth, 1);
    float4 prevClip = mul(currentNDC, gNDCToPrevNDC);
    float2 prevNdcPos = prevClip.xy / prevClip.w;
    float2 prevUV = NDCToUV(prevNdcPos);

    // TODO - Scale blue length by framerate
    // TODO - Clamp to maximum blur radius
    float2 blurDir = (prevUV - currentUV) * 0.5;
    float2 testdir = float2(0.5, 0.5);

    float4 output = TextureSampling(BilinearSampler, SourceMap, SourceMapMS, currentUV, gMSAACount);
    for (i = -gHalfNumSamples; i < 0; ++i) 
    {
        float2 offset = blurDir;
        // float2 offset = blurDir * (i / (float)gHalfNumSamples);
        output += TextureSampling(BilinearSampler, SourceMap, SourceMapMS, currentUV, gMSAACount);
    }

    for (i = 1; i <= gHalfNumSamples; ++i) 
    {
        float2 offset = blurDir;
        // float2 offset = blurDir * (i / (float)gHalfNumSamples);
        output += TextureSampling(BilinearSampler, SourceMap, SourceMapMS, currentUV, gMSAACount);
    }

    output /= gHalfNumSamples * 2 + 1;
    return output;
}
