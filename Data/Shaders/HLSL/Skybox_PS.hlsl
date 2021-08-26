cbuffer PerFrameBuffer : register(b0)
{
    float4 gClearColor;
    float gBrightness;
    uint gUseTexture;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 Direction : TEXCOORD0;
};

SamplerState BilinearSampler : register(s0);
TextureCube TextureMap : register(t0);

float4 main( PS_INPUT IN ) : SV_Target
{
    if(gUseTexture == 0)
        return gClearColor * gBrightness;
    else
        return TextureMap.SampleLevel(BilinearSampler, IN.Direction, 0) * gBrightness;
}
