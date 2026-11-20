cbuffer PerShadowBuffer : register(b0)
{
    float4x4 gMatViewProj;
    float2 gNDCZToDeviceZ;
    float gDepthBias;
    float gInvDepthRange;
    float4 gShadowLightDirection;
    float4 gShadowLightPosition;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float ShadowPosition : TEXCOORD0;
};

float4 main(PS_INPUT IN, out float OUT : SV_Depth) : SV_Target0
{
    OUT = saturate(IN.ShadowPosition * gInvDepthRange + gDepthBias);
    return 0.0;
}
