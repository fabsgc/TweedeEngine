cbuffer PerFrameBuffer : register(b0)
{
    uint gMSAACount;
    uint gIsDepth;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

SamplerState BilinearSampler : register(s0);

Texture2D SourceMap : register(t0);
Texture2DMS<float4> SourceMapMS : register(t1);
Texture2DMS<float> SourceMapMSDepth : register(t2);

float4 main( PS_INPUT IN ) : SV_Target0
{
    if(gMSAACount > 1)
    {
        float4 sum = float4(0, 0, 0, 0);

        for(uint i = 0; i < gMSAACount; i++)
        {
            if(gIsDepth == 0) sum += SourceMapMS.Load(IN.Texture, i);
            else sum += SourceMapMSDepth.Load(IN.Texture, i);
        }

        return sum / gMSAACount;
    }

    return SourceMap.Sample(BilinearSampler, IN.Texture);
}
