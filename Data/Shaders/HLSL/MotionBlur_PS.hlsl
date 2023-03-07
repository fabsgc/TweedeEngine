#include "Include/PostProcess.hlsli"

#define BLUR_CAMERA_ONLY 0
#define BLUR_OBJECT_ONLY 1
#define BLUR_CAMERA_AND_OBJECT 2

cbuffer PerCameraBuffer : register(b0)
{
    CameraData gCamera;
}

cbuffer PerFrameBuffer : register(b1)
{
    float gFrameDelta;
    int gHalfNumSamples;
    int gBlurType;
    uint gMSAACount;
}

/** Converts position in NDC to UV coordinates mapped to the screen rectangle. */ 
float2 NDCToUV(float2 ndcPos)
{
    return ndcPos.xy * gCamera.ClipToUVScaleOffset.xy + gCamera.ClipToUVScaleOffset.zw;
}

SamplerState Sampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);

Texture2D DepthMap : register(t2);
Texture2DMS<float4> DepthMapMS : register(t3);

Texture2D VelocityMap : register(t4);
Texture2DMS<float4> VelocityMapMS : register(t5);

static const float FrameDelta = 1 / 60.0;

float4 ComputeMotionBlur(float2 currentUV, float2 blurDir)
{
    float4 output = (float4)0;
    float step = 1.0 / (float)gHalfNumSamples;

    for (int i = -gHalfNumSamples; i < 0; ++i) 
    {
        float2 offset = blurDir * step * i;
        float2 uv = currentUV + offset;

        output += TextureLevelSampling(Sampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    for (int j = 1; j <= gHalfNumSamples; ++j) 
    {
        float2 offset = blurDir * step * j;
        float2 uv = currentUV + offset;

        output += TextureLevelSampling(Sampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    return output;
}

float4 ComputeObjectMotionBlur(float2 currentUV, float2 velocity)
{
    float4 output = (float4)0;
    float step = 1.5 / (float)(gHalfNumSamples * 2.0);

    for (int i = 1; i <= gHalfNumSamples * 2; ++i)
    {
        float2 offset = velocity * i * step;
        float2 uv = currentUV + offset;

        output += TextureLevelSampling(Sampler, SourceMap, SourceMapMS, uv, gMSAACount, 0);
    }

    return output;
}

float4 main( PS_INPUT IN ) : SV_Target0
{
    float fixDelta = gFrameDelta / FrameDelta;
    float2 currentUV = IN.Texture;
    float2 ndcPos = IN.ScreenPosition;
    float4 output = (float4)0;
    uint blurPasses = 1;
    // float curDepth = TextureSampling(Sampler, DepthMap, DepthMapMS, currentUV, gMSAACount).r;

    output = TextureSampling(Sampler, SourceMap, SourceMapMS, currentUV, gMSAACount);

    // ##### CAMERA MOTION BLUR
    if(gBlurType == BLUR_CAMERA_ONLY || gBlurType == BLUR_CAMERA_AND_OBJECT)
    {
        float4 currentNDC = float4(ndcPos, 1, 1);
        float4 prevClip = mul(gCamera.NDCToPrevNDC, currentNDC);
        float2 prevNdcPos = prevClip.xy / prevClip.w;
        float2 prevUV = NDCToUV(prevNdcPos);

        float2 cameraBlurDir = (prevUV - currentUV) * fixDelta;

        while(abs(length(cameraBlurDir)) > 0.01)
        {
            cameraBlurDir /= 2.0;
        }

        if(length(cameraBlurDir) > 0.001f)
        {
            output += ComputeMotionBlur(currentUV, cameraBlurDir);
            blurPasses += gHalfNumSamples * 2;
        }
    }

    // ##### OBJECT MOTION BLUR
    if(gBlurType == BLUR_OBJECT_ONLY || gBlurType == BLUR_CAMERA_AND_OBJECT)
    {
        float2 velocity = TextureSampling(Sampler, VelocityMap, VelocityMapMS, currentUV, gMSAACount).xy;
        velocity = DecodeVelocity16SNORM(velocity);

        if(length(velocity) > 0.001f)
        {
            output += ComputeObjectMotionBlur(currentUV, velocity);
            blurPasses += gHalfNumSamples * 2;
        }
    }

    if(blurPasses > 1)
        output /= blurPasses;

    return output;
}
