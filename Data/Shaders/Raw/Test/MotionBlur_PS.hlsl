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
    float4 gClipToUVScaleOffset;
    float4 gUVToClipScaleOffset;
}

cbuffer PerFrameBuffer : register(b1)
{
    float gFrameDelta;
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

Texture2D VelocityMap : register(t4);
Texture2DMS<float4> VelocityMapMS : register(t5);

static const float FrameDelta = 1 / 60.0;

float4 ComputeMotionBlur(float4 input, float2 currentUV, float2 blurDir)
{
    int i = 0;
    float4 output = input;

    for (i = -gHalfNumSamples; i < 0; ++i) 
    {
        float2 offset = blurDir * (i / (float)gHalfNumSamples);
        float2 uv = currentUV + offset;

        if(uv.x > 1.0) uv.x = 0.991;
        if(uv.x < 0.0) uv.x = 0.001;
        if(uv.y > 1.0) uv.y = 0.991;
        if(uv.y < 0.0) uv.y = 0.001;

        output += TextureSampling(BilinearSampler, SourceMap, SourceMapMS, uv, gMSAACount);
    }

    for (i = 1; i <= gHalfNumSamples; ++i) 
    {
        float2 offset = blurDir * (i / (float)gHalfNumSamples);
        float2 uv = currentUV + offset;

        if(uv.x > 1.0) uv.x = 0.991;
        if(uv.x < 0.0) uv.x = 0.001;
        if(uv.y > 1.0) uv.y = 0.991;
        if(uv.y < 0.0) uv.y = 0.001;

        output += TextureSampling(BilinearSampler, SourceMap, SourceMapMS, uv, gMSAACount);
    }

    return output;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float2 currentUV = IN.Texture;
    float2 ndcPos = IN.ScreenPosition;

    // float curDepth = TextureSampling(BilinearSampler, DepthMap, DepthMapMS, currentUV, gMSAACount).r;

    float4 currentNDC = float4(ndcPos, 1, 1);
    float4 prevClip = mul(currentNDC, gNDCToPrevNDC);
    float2 prevNdcPos = prevClip.xy / prevClip.w;
    float2 prevUV = NDCToUV(prevNdcPos);

    float fixDelta = FrameDelta / gFrameDelta;

    // ##### CAMERA MOTION BLUR
    float2 cameraBlurDir = (prevUV - currentUV) * 0.5 * fixDelta;

    while(abs(length(cameraBlurDir)) > 0.05)
    {
        cameraBlurDir /= 2.0;
    }

    float4 output = TextureSampling(BilinearSampler, SourceMap, SourceMapMS, currentUV, gMSAACount);
    output = ComputeMotionBlur(output, currentUV, cameraBlurDir);

    output /= gHalfNumSamples * 2 + 1;

    // ##### OBJECT MOTION BLUR
    float2 objectBlurDir = (float2)0;

    return output;
}
