#ifndef __COMMON__
#define __COMMON__

struct CameraData
{
    float3 ViewDir;
    float  ViewportX;
    float3 ViewOrigin;
    float  ViewportY;
    matrix MatViewProj;
    matrix MatView;
    matrix MatProj;
    matrix MatPrevViewProj;
    matrix NDCToPrevNDC;
    float4 ClipToUVScaleOffset;
    float4 UVToClipScaleOffset;
    uint   UseSRGB;
    float3 Padding;
};

#endif // __COMMON__
