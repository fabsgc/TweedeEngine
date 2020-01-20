struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Specular : COLOR0;
    float4 Color : COLOR1;
};

SamplerState AnisotropicColorSampler : register(s0)
{
    Filter = COMPARISON_ANISOTROPIC;
    MaxAnisotropy = 8;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MinLOD = 0;
    MaxLOD = FLOAT32_MAX;
};

Texture2D SpecularTexture : register(t0);

float4 main( PS_INPUT IN ) : SV_Target
{
    float2 texturePosition = (float2)0;
    texturePosition.x = 0.1;
    texturePosition.y = 0.1;

    float4 color = IN.Color;
    float4 specular = IN.Specular * SpecularTexture.Sample(AnisotropicColorSampler, texturePosition);

    if(specular.x > 0.5 && specular.y > 0.5 && specular.z > 0.5 && specular.z > 0.5)
    {
        color *= specular;
    }

    return color;
}
