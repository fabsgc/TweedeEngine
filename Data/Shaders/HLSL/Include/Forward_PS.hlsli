// #################### DEFINES

#define MAX_LIGHTS 24

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

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
    uint   UseIBL;
    uint   UseDiffuseIrrMap;
    float2 Padding1;
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
    bool3  Padding2;
    float  Padding3;
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
    uint   gUseSkyboxDiffuseIrrMap;
    float4 gSceneLightColor;
    float  gSkyboxBrightness;
    float3 gPadding2;
}

SamplerState TextureSampler : register(s0);

TextureCube DiffuseIrrMap : register(t0);

// #################### HELPER FUNCTIONS

float4 ComputeNormalBuffer(float4 normal)
{
    return float4(normal.xyz, 1.0);
}

float3 DoDiffuseIBL(float3 N)
{
    float3 result = (float3)0;

    if(gMaterial.UseDiffuseIrrMap || gUseSkyboxDiffuseIrrMap)
        result = DiffuseIrrMap.Sample(TextureSampler, N).rgb * gSkyboxBrightness * 1.75f;

    return result;
}
