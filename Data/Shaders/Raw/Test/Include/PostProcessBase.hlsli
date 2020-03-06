struct VS_INPUT
{
    float3 ScreenPosition : POSITION;
    float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float2 ScreenPosition : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
    float2 ScreenPosition : TEXCOORD1;
};

float4 TextureSampling(SamplerState samplterState, Texture2D source, 
    Texture2DMS<float4> sourceMS, float2 uv, uint MSAACount)
{
    float4 color = (float4)0;

    if(MSAACount > 1)
    {
        float4 sum = float4(0, 0, 0, 0);
        for(uint i = 0; i < MSAACount; i++)
            sum += sourceMS.Load(uv, i);
        color = sum / MSAACount;
    }
    else
        color = source.Sample(samplterState, uv);

    return color;
}
