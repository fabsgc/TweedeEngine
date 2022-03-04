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
    float4 BaseColor;
    float  Metallic;
    float  Roughness;
    float  Reflectance;
    float  AmbientOcclusion;
    float4 Emissive;
};

struct LightData
{
    float3 Color;
    float  Type;
    float3 Position;
    float  Intensity;
    float3 Direction;
    float  AttenuationRadius;
    float3 SpotAngles;
    float  BoundsRadius;
    float  LinearAttenuation;
    float  QuadraticAttenuation;
    bool   CastShadows;
    bool3  Padding1;
    float  Padding2;
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
    float3    gPadding1;
}

cbuffer PerFrameBuffer : register(b3)
{
    float  gTime;
    float  gFrameDelta;
    uint   gUseSkyboxMap;
    uint   gUseSkyboxIrradianceMap;
    float4 gSceneLightColor;
    float  gSkyboxBrightness;
    float3 gPadding2;
}

// #################### HELPER FUNCTIONS

float4 ComputeNormalBuffer(float4 normal)
{
    return float4(normal.xyz, 1.0);
}
