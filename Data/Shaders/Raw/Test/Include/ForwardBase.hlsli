#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128
#define MAX_LIGHTS 16

struct PerInstanceData
{
    matrix gMatWorld;
    matrix gMatInvWorld;
    matrix gMatWorldNoScale;
    matrix gMatInvWorldNoScale;
    matrix gMatPrevWorld;
    uint   gLayer;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 BiTangent : BINORMAL;
    float2 Texture : TEXCOORD0;
    float4 Color : COLOR0;
    uint   Instanceid : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BINORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection : POSITION1;
    float4 WorldViewDistance : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BINORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection: POSITION1;
    float4 WorldViewDistance : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 Scene : SV_Target0;
    //float4 Albedo : SV_Target1;
    //float4 Specular : SV_Target2;
    float4 Normal : SV_Target1;
    float4 Emissive : SV_Target2;
    //float4 Velocity : SV_Target5;
};

struct LightData
{
    float3 Color;
    float Type;
    float3 Position;
    float Intensity;
    float3 Direction;
    float AttenuationRadius;
    float3 SpotAngles;
    float BoundsRadius;
};

float2 FlipUV(float2 coord)
{
    return float2(coord.x - 1.0f, coord.y);
}

float3 ExpandNormal( float3 n )
{
    return n * 2.0f - 1.0f;
}
 
float3 DoNormalMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv )
{
    float3 normal = tex.Sample( s, uv ).xyz;
    normal = ExpandNormal( normal );
 
    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );
    return normalize( normal );
}

float3 DoBumpMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv, float bumpScale )
{
    // Sample the heightmap at the current texture coordinate.
    float height = tex.Sample( s, uv ).r * bumpScale;
    // Sample the heightmap in the U texture coordinate direction.
    float heightU = tex.Sample( s, uv, int2( 1, 0 ) ).r * bumpScale;
    // Sample the heightmap in the V texture coordinate direction.
    float heightV = tex.Sample( s, uv, int2( 0, 1 ) ).r * bumpScale;
 
    float3 p = { 0, 0, height };
    float3 pU = { 0.2, 0, heightU };
    float3 pV = { 0, 0.2, heightV };
 
    // normal = tangent x bitangent
    float3 normal = cross( normalize(pU - p), normalize(pV - p) );
 
    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );
 
    return normal;
}
