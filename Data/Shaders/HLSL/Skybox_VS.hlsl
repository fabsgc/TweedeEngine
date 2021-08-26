cbuffer PerCameraBuffer : register(b0)
{
    float3 gViewDir;
    float3 gViewOrigin;
    matrix gMatViewProj;
    matrix gMatView;
    matrix gMatProj;
    matrix gMatPrevViewProj;
}

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Direction : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 pos = mul(gMatViewProj, float4(IN.Position.xyz + gViewOrigin, 1));

    // Set Z = W so that final depth is 1.0f and it renders behind everything else
    OUT.Position = pos.xyww;
    OUT.Direction = IN.Position;

    return OUT;
}
