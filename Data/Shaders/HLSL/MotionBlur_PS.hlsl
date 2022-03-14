#include "Include/PostProcess.hlsli"

cbuffer PerCameraBuffer : register(b0)
{
    CameraData gCamera;
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
    return ndcPos.xy * gCamera.ClipToUVScaleOffset.xy + gCamera.ClipToUVScaleOffset.zw;
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

        output += TextureLevelSampling(BilinearSampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    for (i = 1; i <= gHalfNumSamples; ++i) 
    {
        float2 offset = blurDir * (i / (float)gHalfNumSamples);
        float2 uv = currentUV + offset;

        if(uv.x > 1.0) uv.x = 0.991;
        if(uv.x < 0.0) uv.x = 0.001;
        if(uv.y > 1.0) uv.y = 0.991;
        if(uv.y < 0.0) uv.y = 0.001;

        output += TextureLevelSampling(BilinearSampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    return output;
}

float4 ComputeObjectMotionBlur(float4 input, float2 currentUV, float2 blurDir)
{
    int i = 0;
    float4 output = input;

    for (i = 1; i <= gHalfNumSamples; ++i) 
    {
        float2 offset = blurDir * (i / (float)gHalfNumSamples);
        float2 uv = currentUV + offset;

        if(uv.x > 1.0) uv.x = 0.991;
        if(uv.x < 0.0) uv.x = 0.001;
        if(uv.y > 1.0) uv.y = 0.991;
        if(uv.y < 0.0) uv.y = 0.001;

        output += TextureLevelSampling(BilinearSampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    return output;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float fixDelta = gFrameDelta / FrameDelta;
    float2 currentUV = IN.Texture;
    float2 ndcPos = IN.ScreenPosition;
    float4 output = (float4)0;
    uint blurPass = 0;
    // float curDepth = TextureSampling(BilinearSampler, DepthMap, DepthMapMS, currentUV, gMSAACount).r;

    output = TextureSampling(BilinearSampler, SourceMap, SourceMapMS, currentUV, gMSAACount);

    // ##### CAMERA MOTION BLUR
    float4 currentNDC = float4(ndcPos, 1, 1);
    float4 prevClip = mul(gCamera.NDCToPrevNDC, currentNDC);
    float2 prevNdcPos = prevClip.xy / prevClip.w;
    float2 prevUV = NDCToUV(prevNdcPos);

    float2 cameraBlurDir = (prevUV - currentUV) * fixDelta;

    blurPass += 2;
    while(abs(length(cameraBlurDir)) > 0.01)
    {
        cameraBlurDir /= 2.0;
    }

    output = ComputeMotionBlur(output, currentUV, cameraBlurDir);

    // ##### OBJECT MOTION BLUR
    float2 objectBlurDir = TextureSampling(BilinearSampler, VelocityMap, VelocityMapMS, currentUV, gMSAACount).xy;
    if(abs(objectBlurDir.x) >= 0.51 || abs(objectBlurDir.y) >= 0.51)
    {
        objectBlurDir -= 0.5;
        objectBlurDir *= 2.0;

        objectBlurDir = objectBlurDir * fixDelta;

        if(abs(length(objectBlurDir)) > 0.01f)
        {
            blurPass += 1;
            while(abs(length(objectBlurDir)) > 0.5)
            {
                objectBlurDir /= 2.0;
            }

            output = ComputeObjectMotionBlur(output, currentUV, objectBlurDir);
        }
    }

    output /= gHalfNumSamples * blurPass + 1;

    return output;
}
