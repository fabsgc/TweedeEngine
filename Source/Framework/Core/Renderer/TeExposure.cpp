#include "TeExposure.h"

#include "Renderer/TeCamera.h"

namespace te
{
    float Exposure::GetEv100(const Camera& camera) noexcept
    {
        return GetEv100(camera.GetAperture(), camera.GetShutterSpeed(), static_cast<float>(camera.GetSensitivity()));
    }

    float Exposure::GetEv100(float aperture, float shutterSpeed, float sensitivity) noexcept
    {
        // With N = aperture, t = shutter speed and S = sensitivity,
        // we can compute EV100 knowing that:
        //
        // EVs = log2(N^2 / t)
        // and
        // EVs = EV100 + log2(S / 100)
        //
        // We can therefore find:
        //
        // EV100 = EVs - log2(S / 100)
        // EV100 = log2(N^2 / t) - log2(S / 100)
        // EV100 = log2((N^2 / t) * (100 / S))
        //
        // Reference: https://en.wikipedia.org/wiki/Exposure_value
        return Math::Log((aperture * aperture) / shutterSpeed * 100.0f / sensitivity);
    }

    float Exposure::GetEv100FromLuminance(float luminance) noexcept
    {
        // With L the average scene luminance, S the sensitivity and K the
        // reflected-light meter calibration constant:
        //
        // EV = log2(L * S / K)
        //
        // With the usual value K = 12.5 (to match standard camera manufacturers
        // settings), computing the EV100 becomes:
        //
        // EV100 = log2(L * 100 / 12.5)
        //
        // Reference: https://en.wikipedia.org/wiki/Exposure_value
        return Math::Log2(luminance * (100.0f / 12.5f));
    }

    float Exposure::GetEv100FromIlluminance(float illuminance) noexcept
    {
        // With E the illuminance, S the sensitivity and C the incident-light meter
        // calibration constant, the exposure value can be computed as such:
        //
        // EV = log2(E * S / C)
        // or
        // EV100 = log2(E * 100 / C)
        //
        // Using C = 250 (a typical value for a flat sensor), the relationship between
        // EV100 and illuminance is:
        //
        // EV100 = log2(E * 100 / 250)
        //
        // Reference: https://en.wikipedia.org/wiki/Exposure_value
        return Math::Log2(illuminance * (100.0f / 250.0f));
    }

    float Exposure::GetExposure(const Camera& camera) noexcept
    {
        return GetExposure(camera.GetAperture(), camera.GetShutterSpeed(), static_cast<float>(camera.GetSensitivity()));
    }

    float Exposure::GetExposure(float aperture, float shutterSpeed, float sensitivity) noexcept
    {
        // This is equivalent to calling exposure(ev100(N, t, S))
        // By merging the two calls we can remove extra pow()/log2() calls
        const float e = (aperture * aperture) / shutterSpeed * 100.0f / sensitivity;
        return 1.0f / (1.2f * e);
    }

    float Exposure::GetExposure(float ev100) noexcept
    {
        // The photometric exposure H is defined by:
        //
        // H = (q * t / (N^2)) * L
        //
        // Where t is the shutter speed, N the aperture, L the incident luminance
        // and q the lens and vignetting attenuation. A typical value of q is 0.65
        // (see reference link below).
        //
        // The value of H as recorded by a sensor depends on the sensitivity of the
        // sensor. An easy way to find that value is to use the saturation-based
        // sensitivity method:
        //
        // S_sat = 78 / H_sat
        //
        // This method defines the maximum possible exposure that does not lead to
        // clipping or blooming.
        //
        // The factor 78 is chosen so that exposure settings based on a standard
        // light meter and an 18% reflective surface will result in an image with
        // a grey level of 18% * sqrt(2) = 12.7% of saturation. The sqrt(2) factor
        // is used to account for an extra half a stop of headroom to deal with
        // specular reflections.
        //
        // Using the definitions of H and S_sat, we can derive the formula to
        // compute the maximum luminance to saturate the sensor:
        //
        // H_sat = 78 / S_stat
        // (q * t / (N^2)) * Lmax = 78 / S
        // Lmax = (78 / S) * (N^2 / (q * t))
        // Lmax = (78 / (S * q)) * (N^2 / t)
        //
        // With q = 0.65, S = 100 and EVs = log2(N^2 / t) (in this case EVs = EV100):
        //
        // Lmax = (78 / (100 * 0.65)) * 2^EV100
        // Lmax = 1.2 * 2^EV100
        //
        // The value of a pixel in the fragment shader can be computed by
        // normalizing the incident luminance L at the pixel's position
        // with the maximum luminance Lmax
        //
        // Reference: https://en.wikipedia.org/wiki/Film_speed
        return 1.0f / (1.2f * Math::Pow(2.0f, ev100));
    }

    float Exposure::GetLuminance(const Camera& camera) noexcept
    {
        return GetLuminance(camera.GetAperture(), camera.GetShutterSpeed(), static_cast<float>(camera.GetSensitivity()));
    }

    float Exposure::GetLuminance(float aperture, float shutterSpeed, float sensitivity) noexcept
    {
        // This is equivalent to calling luminance(ev100(N, t, S))
        // By merging the two calls we can remove extra pow()/log2() calls
        const float e = (aperture * aperture) / shutterSpeed * 100.0f / sensitivity;
        return e * 0.125f;
    }

    float Exposure::GetLuminance(float ev100) noexcept
    {
        // With L the average scene luminance, S the sensitivity and K the
        // reflected-light meter calibration constant:
        //
        // EV = log2(L * S / K)
        // L = 2^EV100 * K / 100
        //
        // As in ev100FromLuminance(luminance), we use K = 12.5 to match common camera
        // manufacturers (Canon, Nikon and Sekonic):
        //
        // L = 2^EV100 * 12.5 / 100 = 2^EV100 * 0.125
        //
        // With log2(0.125) = -3 we have:
        //
        // L = 2^(EV100 - 3)
        //
        // Reference: https://en.wikipedia.org/wiki/Exposure_value
        return Math::Pow(2.0f, ev100 - 3.0f);
    }

    float Exposure::GetIlluminance(const Camera& camera) noexcept
    {
        return GetIlluminance(camera.GetAperture(), camera.GetShutterSpeed(), static_cast<float>(camera.GetSensitivity()));
    }

    float Exposure::GetIlluminance(float aperture, float shutterSpeed, float sensitivity) noexcept
    {
        // This is equivalent to calling illuminance(ev100(N, t, S))
        // By merging the two calls we can remove extra pow()/log2() calls
        const float e = (aperture * aperture) / shutterSpeed * 100.0f / sensitivity;
        return 2.5f * e;
    }

    float Exposure::GetIlluminance(float ev100) noexcept
    {
        // With E the illuminance, S the sensitivity and C the incident-light meter
        // calibration constant, the exposure value can be computed as such:
        //
        // EV = log2(E * S / C)
        // or
        // EV100 = log2(E * 100 / C)
        //
        // Using C = 250 (a typical value for a flat sensor), the relationship between
        // EV100 and illuminance is:
        //
        // EV100 = log2(E * 100 / 250)
        // E = 2^EV100 / (100 / 250)
        // E = 2.5 * 2^EV100
        //
        // Reference: https://en.wikipedia.org/wiki/Exposure_value
        return 2.5f * Math::Pow(2.0f, ev100);
    }
}
