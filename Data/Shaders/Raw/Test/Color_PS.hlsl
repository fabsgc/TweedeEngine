cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
}

cbuffer PerMaterialBuffer : register(b1)
{
    float4 gAmbient;
    float4 gDiffuse;
    float4 gEmissive;
    float4 gSpecular;
    uint   gUseDiffuseMap;
    uint   gUseNormalMap;
    uint   gUseBumpMap;
    uint   gUseSpecularMap;
    float  gSpecularPower;
    float  gTransparency;
    float  gAbsorbance;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR0;
    float2 Texture : TEXCOORD0;
    float3 ViewDirection: POSITION1;
};

SamplerState AnisotropicSampler : register(s0);
Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D BumpMap : register(t3);

static const float4 AmbientColor = float4(1.0f, 0.95f, 0.9f, 0.5f);
static const float3 AmbientDirection = float3(1.0f, -2.0f, -2.0f);

static const float4 SpecularColor = float4(1.0f, 0.95f, 0.9f, 4.0f);
static const float SpecularPower = 64.0f;

float4 main( PS_INPUT IN ) : SV_Target
{
    float4 outColor  = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 color     = Color;

    float3 diffuse   = (float3)0;
    float3 specular  = (float3)0;
    float3 refVector = (float3)0;
    float3 ambient   = AmbientColor.rgb * AmbientColor.a;

    float3 ambientDirection = normalize(-AmbientDirection);
    float  n_dot_l          = dot(ambientDirection, IN.Normal);

    if(n_dot_l > 0)
    {
        // D = kd * ld * md
        diffuse = (max(n_dot_l, 0) * color.rgb);
        // R = I - 2(n.I) * n
        refVector = normalize(reflect(ambientDirection, IN.Normal));
        // S = max(dot(V.R),0)^P * SpecularColor.rgb * SpecularColor.a * color.rgb;
        specular = pow(max(dot(IN.ViewDirection, refVector), 0), SpecularPower.x) * SpecularColor.rgb * SpecularColor.a;
    }
    
    outColor.rgb = color.rgb * (ambient.rgb + diffuse.rgb) + specular.rgb;
    return outColor;
}
