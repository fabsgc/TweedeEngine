//------------------------------------------------------------------------------
// Common graphics
//------------------------------------------------------------------------------

float3 ApproximationSRgbToLinear (in float3 sRGBCol )
{
    return pow ( sRGBCol , 2.2);
}

float3 ApproximationLinearToSRGB (in float3 linearCol )
{
    return pow ( linearCol , 1 / 2.2);
}

float3 AccurateSRGBToLinear (in float3 sRGBCol )
{
    float3 linearRGBLo = sRGBCol / 12.92;
    float3 linearRGBHi = pow (( sRGBCol + 0.055) / 1.055 , 2.4);
    float3 linearRGB = ( sRGBCol <= 0.04045) ? linearRGBLo : linearRGBHi;
    return linearRGB;
}

float3 AccurateLinearToSRGB (in float3 linearCol )
{
    float3 sRGBLo = linearCol * 12.92;
    float3 sRGBHi = ( pow( abs ( linearCol ) , 1.0/2.4) * 1.055) - 0.055;
    float3 sRGB = ( linearCol <= 0.0031308) ? sRGBLo : sRGBHi;
    return sRGB;
}