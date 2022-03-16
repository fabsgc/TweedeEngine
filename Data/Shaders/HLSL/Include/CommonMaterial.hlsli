#ifndef __COMMONMATERIAL__
#define __COMMONMATERIAL__

float F0ToIor(float f0) 
{
    float r = sqrt(f0);
    return (1.0 + r) / (1.0 - r);
}

#endif // __COMMONMATERIAL__