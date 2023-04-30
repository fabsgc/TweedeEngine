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

    // Transforms a location in NDC, to the location of the same pixel on the previous frame. Used for
    // determining camera movement for temporal filtering
    matrix NDCToPrevNDC;

    // Converts device Z to world Z using this formula: worldZ = (1 / (deviceZ + y)) * x
    float2 DeviceZToWorldZ;
    float2 NDCZToWorldZ;
    float2 NDCZToDeviceZ;
    
    // x - near plane distance, y - far plane distance
    float2 NearFar;

    // xy - (Viewport size in pixels / 2) / Target size in pixels
    // zw - (Viewport offset in pixels + (Viewport size in pixels / 2) + Optional pixel center offset) / Target size in pixels
    float4 ClipToUVScaleOffset;
    float4 UVToClipScaleOffset;

    uint   UseSRGB;

    float  Exposure; // Exposure computed from camera Aperture, ShutterSpeed and ISO

    float2 Padding;
};

/** Converts Z value in range [0,1] into Z value in view space. */
float ConvertFromDeviceZ(CameraData camera, float deviceZ)
{
    // Note: Convert to MAD form
    return camera.DeviceZToWorldZ.x / (deviceZ + camera.DeviceZToWorldZ.y);
}

/** Converts Z value in range [0,1] into Z value in view space. */
float4 ConvertFromDeviceZ(CameraData camera, float4 deviceZ)
{
    // Note: Convert to MAD form
    return camera.DeviceZToWorldZ.x / (deviceZ + camera.DeviceZToWorldZ.y);
}

/** Converts Z value from view space to NDC space. */
float ConvertToNDCZ(CameraData camera, float viewZ)
{
    return -camera.NDCZToWorldZ.y + (camera.NDCZToWorldZ.x / viewZ);
}

/** Converts Z value from NDC space to device Z value in range [0, 1]. */
float NDCZToDeviceZ(CameraData camera, float ndcZ)
{
    return (ndcZ + camera.NDCZToDeviceZ.y) * camera.NDCZToDeviceZ.x;
}

/** Converts Z value from device range ([0, 1]) to NDC space. */
float DeviceZToNDCZ(CameraData camera, float deviceZ)
{
    return deviceZ / camera.NDCZToDeviceZ.x - camera.NDCZToDeviceZ.y;
}

/** Converts position in NDC to UV coordinates mapped to the screen rectangle. */ 
float2 NDCToUV(CameraData camera, float2 ndcPos)
{
    return ndcPos.xy * camera.ClipToUVScaleOffset.xy + camera.ClipToUVScaleOffset.zw;
}

/** Converts position in UV coordinates mapped to screen rectangle to NDC coordinates. */
float2 UVToNDC(CameraData camera, float2 uvPos)
{
    return uvPos * camera.UVToClipScaleOffset.xy + camera.UVToClipScaleOffset.zw;
}

// Encodes velocity into a format suitable for storing in a 16-bit SNORM texture. 
// Velocity range of [-2, 2] is supported (full NDC).
float2 EncodeVelocity16SNORM(float2 velocity)
{
    return velocity * 0.5f;
}

// Decodes velocity from an encoded 16-bit SNORM format. See encodeVelocity16SNORM().
// Velocity range of [-2, 2] is supported (full NDC).
float2 DecodeVelocity16SNORM(float2 val)
{
    return val * 2.0f;
}

#endif // __COMMON__
