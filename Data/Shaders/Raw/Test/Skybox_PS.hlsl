cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
    matrix gMatPrevViewProj;
}

cbuffer PerFrameBuffer : register(b1)
{
    float4 gClearColor;
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
        return gClearColor;
    else
        return TextureMap.SampleLevel(BilinearSampler, IN.Direction, 0);
}
