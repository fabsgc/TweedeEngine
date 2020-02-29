#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Settings that control automatic exposure (eye adaptation) post-process. */
    struct TE_CORE_EXPORT AutoExposureSettings
    {
        AutoExposureSettings() = default;
    };

    /** Settings that control tonemap post-process. */
    struct TE_CORE_EXPORT TonemappingSettings
    {
        TonemappingSettings() = default;
    };

    /** Settings that control screen space ambient occlusion. */
    struct TE_CORE_EXPORT AmbientOcclusionSettings
    {
        AmbientOcclusionSettings() = default;

        /** Enables or disables the screen space ambient occlusion effect. */
        bool Enabled = true;
    };

    /** Base class for both sim and core thread variants of DepthOfFieldSettings. */
    struct TE_CORE_EXPORT DepthOfFieldSettings
    {
        DepthOfFieldSettings() = default;

        /** Enables or disables the depth of field effect. */
        bool Enabled = false;

        /**
         * Determines the amount of blur to apply to fully unfocused objects that are closer to camera than the in-focus
         * zone. Set to zero to disable near-field blur. Only relevant for Gaussian depth of field.
         */
        float NearBlurAmount = 0.02f;

        /**
         * Determines the amount of blur to apply to fully unfocused objects that are farther away from camera than the
         * in-focus zone. Set to zero to disable far-field blur. Only relevant for Gaussian depth of field.
         */
        float FarBlurAmount = 0.02f;
    };

    /** Settings that control the motion blur effect. */
    struct TE_CORE_EXPORT MotionBlurSettings
    {
        MotionBlurSettings() = default;

        /** Enables or disables the motion blur effect. */
        bool Enabled = true;
    };

    /** Settings that control the bloom effect. Bloom adds an extra highlight to bright areas of the scene. */
    struct TE_CORE_EXPORT BloomSettings
    {
        BloomSettings() = default;

        /** Enables or disables the bloom effect. */
        bool Enabled = true;
    };

    /** Various options that control shadow rendering for a specific view. */
    struct TE_CORE_EXPORT ShadowSettings
    {
        ShadowSettings() = default;
    };

    /** Base class for both sim and core thread variants of RenderSettings. */
    struct TE_CORE_EXPORT RenderSettings
    {
        RenderSettings() = default;

        /**
         * Determines should automatic exposure be applied to the HDR image. When turned on the average scene brightness
         * will be calculated and used to automatically expose the image to the optimal range. Use the parameters provided
         * by autoExposure to customize the automatic exposure effect. You may also use exposureScale to
         * manually adjust the automatic exposure. When automatic exposure is turned off you can use exposureScale to
         * manually set the exposure.
         */
        bool EnableAutoExposure = true;

        /**
         * Parameters used for customizing automatic scene exposure.
         *
         * @see	enableAutoExposure
         */
         AutoExposureSettings AutoExposure; // TODO not used currently

        /**
         * Determines should the image be tonemapped. Tonemapping converts an HDR image into LDR image by applying
         * a filmic curve to the image, simulating the effect of film cameras. Filmic curve improves image quality by
         * tapering off lows and highs, preventing under- and over-exposure. This is useful if an image contains both
         * very dark and very bright areas, in which case the global exposure parameter would leave some areas either over-
         * or under-exposed. Use #tonemapping to customize how tonemapping performed.
         *
         * If this is disabled, then color grading and white balancing will not be enabled either. Only relevant for HDR
         * images.
         */
        bool EnableTonemapping = true;

        /**
         * Parameters used for customizing tonemapping.
         *
         * @see	enableTonemapping
         */
        TonemappingSettings Tonemapping;

        /** Parameters used for customizing screen space ambient occlusion. */
        AmbientOcclusionSettings AmbientOcclusion;

        /** Parameters used for customizing the bloom effect. */
        BloomSettings Bloom;

        /** Parameters used for customizing the motion blur effect. */
        MotionBlurSettings MotionBlur;

        /** Parameters used for customizing the gaussian depth of field effect. */
        DepthOfFieldSettings DepthOfField;

        /** Enables the fast approximate anti-aliasing effect. */
        bool EnableFXAA = true;

        /**
         * Log2 value to scale the eye adaptation by (for example 2^0 = 1). Smaller values yield darker image, while larger
         * yield brighter image. Allows you to customize exposure manually, applied on top of eye adaptation exposure (if
         * enabled). In range [-8, 8].
         */
        float ExposureScale = 1.1f;

        /**
         * Gamma value to adjust the image for. Larger values result in a brighter image. When tonemapping is turned
         * on the best gamma curve for the output device is chosen automatically and this value can by used to merely tweak
         * that curve. If tonemapping is turned off this is the exact value of the gamma curve that will be applied.
         */
        float Gamma = 1.25f;

        /**
         * High dynamic range allows light intensity to be more correctly recorded when rendering by allowing for a larger
         * range of values. The stored light is then converted into visible color range using exposure and a tone mapping
         * operator.
         */
        bool EnableHDR = true;

        /**
         * Determines if scene objects will be lit by lights. If disabled everything will be rendered using their albedo
         * texture with no lighting applied.
         */
        bool EnableLighting = true;

        /** Determines if shadows cast by lights should be rendered. Only relevant if lighting is turned on. */
        bool EnableShadows = true;

        /**
         * Determines if the G-Buffer should contain per-pixel velocity information. This can be useful if you are rendering
         * an effect that requires this information. Note that effects such as motion blur or temporal anti-aliasing
         * might force the velocity buffer to be enabled regardless of this setting.
         */
        bool EnableVelocityBuffer = true;

        /**
         * Signals the renderer to only render overlays (like GUI), and not scene objects. Such rendering doesn't require
         * depth buffer or multi-sampled render targets and will not render any scene objects. This can improve performance
         * and memory usage for overlay-only views.
         */
        bool OverlayOnly = false;

        /**
         * If enabled the camera will use the skybox for rendering the background. A skybox has to be present in the scene.
         * When disabled the camera will use the clear color for rendering the background.
         */
        bool EnableSkybox = true;

        /**
         * The absolute base cull-distance for objects rendered through this camera in world units. Objects will use this
         * distance and apply their own factor to it to determine whether they should be visible.
         */
        float CullDistance = FLT_MAX;
    };
}
