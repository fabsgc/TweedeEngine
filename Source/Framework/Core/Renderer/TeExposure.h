#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class Camera;

    class TE_CORE_EXPORT Exposure
    {
    public:
        /**
         * Returns the exposure value (EV at ISO 100) of the specified camera.
         */
        static float GetEv100(const Camera& camera) noexcept;

        /**
         * Returns the exposure value (EV at ISO 100) of the specified exposure parameters.
         */
        static float GetEv100(float aperture, float shutterSpeed, float sensitivity) noexcept;

        /**
         * Returns the exposure value (EV at ISO 100) for the given average luminance (in @f$ \frac{cd}{m^2} @f$).
         */
        static float GetEv100FromLuminance(float luminance) noexcept;

        /**
        * Returns the exposure value (EV at ISO 100) for the given illuminance (in lux).
        */
        static float GetEv100FromIlluminance(float illuminance) noexcept;

        /**
         * Returns the photometric exposure for the specified camera.
         */
        static float GetExposure(const Camera& camera) noexcept;

        /**
         * Returns the photometric exposure for the specified exposure parameters.
         * This function is equivalent to calling `exposure(ev100(aperture, shutterSpeed, sensitivity))`
         * but is slightly faster and offers higher precision.
         */
        static float GetExposure(float aperture, float shutterSpeed, float sensitivity) noexcept;

        /**
         * Returns the photometric exposure for the given EV100.
         */
        static float GetExposure(float ev100) noexcept;

        /**
         * Returns the incident luminance in @f$ \frac{cd}{m^2} @f$ for the specified camera acting as a spot meter.
         */
        static float GetLuminance(const Camera& camera) noexcept;

        /**
         * Returns the incident luminance in @f$ \frac{cd}{m^2} @f$ for the specified exposure parameters of
         * a camera acting as a spot meter.
         * This function is equivalent to calling `luminance(ev100(aperture, shutterSpeed, sensitivity))`
         * but is slightly faster and offers higher precision.
         */
        static float GetLuminance(float aperture, float shutterSpeed, float sensitivity) noexcept;

        /**
         * Converts the specified EV100 to luminance in @f$ \frac{cd}{m^2} @f$.
         * EV100 is not a measure of luminance, but an EV100 can be used to denote a
         * luminance for which a camera would use said EV100 to obtain the nominally
         * correct exposure
         */
        static float GetLuminance(float ev100) noexcept;

        /**
         * Returns the illuminance in lux for the specified camera acting as an incident light meter.
         */
        static float GetIlluminance(const Camera& camera) noexcept;

        /**
         * Returns the illuminance in lux for the specified exposure parameters of
         * a camera acting as an incident light meter.
         * This function is equivalent to calling `illuminance(ev100(aperture, shutterSpeed, sensitivity))`
         * but is slightly faster and offers higher precision.
         */
        static float GetIlluminance(float aperture, float shutterSpeed, float sensitivity) noexcept;

        /**
         * Converts the specified EV100 to illuminance in lux.
         * EV100 is not a measure of illuminance, but an EV100 can be used to denote an
         * illuminance for which a camera would use said EV100 to obtain the nominally
         * correct exposure.
         */
        static float GetIlluminance(float ev100) noexcept;
    };
}
