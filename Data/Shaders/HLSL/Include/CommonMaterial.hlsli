#ifndef __COMMONMATERIAL__
#define __COMMONMATERIAL__

#define MIN_N_DOT_V 1e-4

#define MAX_ROUGHNESS 0.95
#define MIN_ROUGHNESS 0.01

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