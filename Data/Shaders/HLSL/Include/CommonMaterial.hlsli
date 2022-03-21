#ifndef __COMMONMATERIAL__
#define __COMMONMATERIAL__

#define MIN_N_DOT_V 1e-4

#define MAX_ROUGHNESS 0.99999
#define MIN_ROUGHNESS 0.01

#define DFG_TEXTURE_SIZE 128

float3 ComputeF0(const float3 baseColor, float metallic, float reflectance) 
{
    return baseColor * metallic + (reflectance * (1.0 - metallic));
}

float ComputeDielectricF0(float reflectance)
{
    return 0.16 * reflectance * reflectance;
}

float F0ToIor(float f0) 
{
    float r = sqrt(f0);
    return (1.0 + r) / (1.0 - r);
}

float ClampNoV(float NoV) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return max(NoV, MIN_N_DOT_V);
}

#endif // __COMMONMATERIAL__