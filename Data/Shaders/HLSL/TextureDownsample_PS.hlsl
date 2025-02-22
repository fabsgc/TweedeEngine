struct PS_INPUT
{
    noperspective float4 Position : SV_POSITION;
    noperspective float2 Texture : TEXCOORD0;
};

// MSAA_COUNT (1, 2, 4, 8)

#if MSAA_COUNT > 1
Texture2DMS<float4> SourceMap : register(t0);
#else //MSAA_COUNT
Texture2D SourceMap : register(t0);
SamplerState Sampler : register(s0);
#endif //MSAA_COUNT

float4 main( PS_INPUT IN ) : SV_Target0
{
#if MSAA_COUNT > 1
    int2 uv = trunc(IN.Texture);
    float4 sum = (float4)0;

    [unroll]
    for(uint i = 0; i < MSAA_COUNT; i++)
    {
        sum += SourceMap.Load(uv, i);
    }

    return sum / MSAA_COUNT;
#else // MSAA_COUNT
    return SourceMap.Sample(Sampler, IN.Texture);
#endif // MSAA_COUNT
}
