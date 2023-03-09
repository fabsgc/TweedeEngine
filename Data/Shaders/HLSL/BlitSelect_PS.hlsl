struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
};

SamplerState Sampler : register(s0);
Texture2D ViewportMap : register(t0);
Texture2D OutlineMap : register(t1);
Texture2D BlurredOutlineMap : register(t2);

float4 main( PS_INPUT IN ) : SV_Target0
{
    float4 viewportColor = ViewportMap.SampleLevel(Sampler, IN.Texture, 0);
    float4 outlineColor =  OutlineMap.SampleLevel(Sampler, IN.Texture, 0);
    float4 blurredOutlineColor =  BlurredOutlineMap.SampleLevel(Sampler, IN.Texture, 0);

    float4 output = viewportColor + blurredOutlineColor * outlineColor.a;
    output.a = 1.0;

    return output;
}
