#ifndef __COMMONGRAPHICS__
#define __COMMONGRAPHICS__

//------------------------------------------------------------------------------
// Common graphics
//------------------------------------------------------------------------------

/**
 * Gives an approximation of SRGB color to linear color conversion
 */
float3 ApproximationSRGBToLinear (in float3 sRGBCol )
{
    return pow ( sRGBCol , 2.2);
}

/**
 * Gives an approximation of linear color to SRGB color conversion
 */
float3 ApproximationLinearToSRGB (in float3 linearCol )
{
    return pow ( linearCol , 1 / 2.2);
}

/**
 * Gives an accurate linear color to SRGB color conversion
 */
float3 AccurateSRGBToLinear (in float3 sRGBCol )
{
    float3 linearRGBLo = sRGBCol / 12.92;
    float3 linearRGBHi = pow (( sRGBCol + 0.055) / 1.055 , 2.4);
    float3 linearRGB = ( sRGBCol <= 0.04045) ? linearRGBLo : linearRGBHi;
    return linearRGB;
}

/**
 * Gives an accurate SRGB color to linear color conversion
 */
float3 AccurateLinearToSRGB (in float3 linearCol )
{
    float3 sRGBLo = linearCol * 12.92;
    float3 sRGBHi = ( pow( abs ( linearCol ) , 1.0/2.4) * 1.055) - 0.055;
    float3 sRGB = ( linearCol <= 0.0031308) ? sRGBLo : sRGBHi;
    return sRGB;
}

float4 GetGammaCorrectedColor(float4 color, bool useGamma, bool useToneMapping, float gamma,
    float contrast, float brightness, float exposure)
{
    float4 mapped = color;
    mapped.a = 1.0;

    if(useGamma)
    {
        if(useToneMapping)
        {
            // Exposure tone mapping
            mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-color * exposure);
        }

        // Gamma correction 
        float power = float(1.0 / gamma);
        mapped.x = pow(abs(mapped.x), power);
        mapped.y = pow(abs(mapped.y), power);
        mapped.z = pow(abs(mapped.z), power);
        mapped.w = 1.0;

        // Contrast
        mapped.rgb = ((mapped.rgb - 0.5f) * max(contrast, 0)) + 0.5f;

        // Brightness
        mapped.rgb = mapped.rgb + float3(brightness, brightness, brightness);
    }

    return mapped;
}

#endif // __COMMONGRAPHICS__
