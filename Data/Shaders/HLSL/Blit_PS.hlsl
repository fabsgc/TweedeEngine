struct PS_INPUT
{
    noperspective float4 Position : SV_POSITION;
    noperspective float2 Texture : TEXCOORD0;
};

// MSAA_COUNT (1, 2, 4, 8)
// MODE (0 = Color, 1 = Depth)

#if MSAA_COUNT > 1
    #if MODE == 0
Texture2DMS<float4> SourceMap : register(t0);
    #else // MODE
Texture2DMS<float> SourceMap : register(t0);
    #endif // MODE
#else //MSAA_COUNT
SamplerState Sampler : register(s0);
Texture2D SourceMap : register(t0);
#endif //MSAA_COUNT

#if MODE == 0
float4 main( PS_INPUT IN ) : SV_Target0
#else // MODE
float main( PS_INPUT IN, out float depth : SV_Depth) : SV_Target0
#endif // MODE
{
#if MODE == 0
#if MSAA_COUNT > 1
    float4 sum = (float4)0;
    int2 uv = trunc(IN.Texture);

    [unroll]
    for(uint i = 0; i < MSAA_COUNT; i++)
    {
        sum += SourceMap.Load(uv, i);
    }

    return sum / MSAA_COUNT;
#else // MSAA_COUNT
    return SourceMap.Sample(Sampler, IN.Texture);
#endif // MSAA_COUNT
#else // MODE
#if MSAA_COUNT > 1
    int2 uv = trunc(IN.Texture);
    float minVal = SourceMap.Load(uv, 0);
    
    [unroll]
    for(int i = 1; i < MSAA_COUNT; i++)
    {
        minVal = min(minVal, SourceMap.Load(uv, i));
    }
        
    depth = minVal;
    return depth;
#else // MSAA_COUNT
    return SourceMap.Sample(Sampler, IN.Texture);
#endif // MSAA_COUNT
#endif // MODE
}
