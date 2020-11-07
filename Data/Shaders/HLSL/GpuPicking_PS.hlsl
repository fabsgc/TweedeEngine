struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

float4 main( PS_INPUT IN ) : SV_Target
{
    return IN.Color;
}
