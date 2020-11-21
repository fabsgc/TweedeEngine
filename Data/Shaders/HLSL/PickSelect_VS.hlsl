cbuffer PerFrameBuffer : register(b0)
{
    matrix gMatViewProj;
    float3 gViewOrigin;
    uint   gRenderType;
}

cbuffer PerObjectBuffer : register(b1)
{
    matrix gMatWorld;
    float4 gColor;
}

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT main( VS_INPUT IN )
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    OUT.Position.xyz = IN.Position;
    OUT.Position.w = 1.0f;
    OUT.Position = mul(gMatWorld, OUT.Position);
    OUT.Position = mul(gMatViewProj, OUT.Position);

    OUT.Color = gColor;

    return OUT;
}
