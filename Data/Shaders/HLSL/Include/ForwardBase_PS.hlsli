// #################### DEFINES

#define MAX_LIGHTS 24

#define DIRECTIONAL_LIGHT 0.0
#define POINT_LIGHT 1.0
#define SPOT_LIGHT 2.0

#define PARALLAX_MIN_SAMPLE 8
#define PARALLAX_MAX_SAMPLE 256

// #################### STRUCTS

struct MaterialData
{
    float4 gAlbedo;
};

struct LightData
{
    float3 gColor;
    float  gType;
    float3 gPosition;
    float  gIntensity;
    float3 gDirection;
    float  gAttenuationRadius;
    float3 gSpotAngles;
    float  gBoundsRadius;
    float  gLinearAttenuation;
    float  gQuadraticAttenuation;
    float2 gPadding1;
};

struct LightingResult
{
    float3 Diffuse;
    float3 Specular;
};

// #################### CONSTANT BUFFERS

cbuffer PerMaterialBuffer : register(b1)
{
    MaterialData gMaterial;
}

cbuffer PerLightsBuffer : register(b2)
{
    LightData gLights[MAX_LIGHTS];
    uint      gLightsNumber;
    float3    gPadding4;
}

cbuffer PerFrameBuffer : register(b3)
{
    float  gTime;
    float  gFrameDelta;
    uint   gUseSkyboxMap;
    uint   gUseSkyboxIrradianceMap;
    float4 gSceneLightColor;
    float  gSkyboxBrightness;
    float3 gPadding5;
}

// #################### HELPER FUNCTIONS

float4 ComputeNormalBuffer(float4 normal)
{
    return float4(normal.xyz, 1.0);
}
