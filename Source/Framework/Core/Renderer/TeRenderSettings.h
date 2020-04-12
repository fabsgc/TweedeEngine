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

    /** Determines which parts of the scene will trigger motion blur. */
    enum class TE_CORE_EXPORT MotionBlurDomain
    {
        /** Camera movement and rotation will result in full-screen motion blur. */
        CameraOnly,

        /**
         * Object movement and rotation will result in blurring of the moving object. Can be significantly more
         * expensive than just using camera blur due to the requirement to use a velocity buffer (unless some
         * other effect also requires it, in which case it will be re-used).
         */
        ObjectOnly,

        /** Both the camera movement and object movement will result in motion blur. */
        CameraAndObject
    };

    /** Type of filter to use when filtering samples contributing to a blurred pixel. */
    enum class TE_CORE_EXPORT MotionBlurFilter
    {
        /** Samples will be simply averaged together to create the blurred pixel. */
        Simple,

        /**
         * A more advanced reconstruction filter will be used. This filter provides better blur quality at a
         * performance cost. In particular the filter will improve blurring at object boundaries, allowing blur
         * to extend beyond the object silhouette. It will also try to estimate blurred background and provide
         * better weighting between background, center and foreground samples.
         */
        Reconstruction
    };

    /** Determines the number of samples to take during motion blur filtering. */
    enum class TE_CORE_EXPORT MotionBlurQuality
    {
        /** 4 samples per pixel. */
        VeryLow,
        /** 6 samples per pixel. */
        Low,
        /** 8 samples per pixel. */
        Medium,
        /** 12 samples per pixel. */
        High,
        /** 16 samples per pixel. */
        Ultra
    };

    /** Settings that control the motion blur effect. */
    struct TE_CORE_EXPORT MotionBlurSettings
    {
        MotionBlurSettings() = default;

        /** Enables or disables the motion blur effect. */
        bool Enabled = true;

        /** Determines which parts of the scene will trigger motion blur. */
        MotionBlurDomain Domain = MotionBlurDomain::CameraOnly; // TODO

        /** Type of filter to use when filtering samples contributing to a blurred pixel. */
        MotionBlurFilter Filter = MotionBlurFilter::Reconstruction; // TODO

        /**
         * Determines the number of samples to take during motion blur filtering. Increasing this value will
         * yield higher quality blur at the cost of the performance.
         */
        MotionBlurQuality Quality = MotionBlurQuality::Ultra;
    };

    /** Settings that control the bloom effect. Bloom adds an extra highlight to bright areas of the scene. */
    struct TE_CORE_EXPORT BloomSettings
    {
        BloomSettings() = default;

        /** Enables or disables the bloom effect. */
        bool Enabled = true;

        /** Determines the intensity of the bloom effect. Ideally should be in [0, 4] range but higher values are allowed.*/
        float Intensity = 1.0f;
    };

    /** Various options that control shadow rendering for a specific view. */
    struct TE_CORE_EXPORT ShadowsSettings
    {
        ShadowsSettings() = default;

        /**
         * Maximum distance that directional light shadows are allowed to render at. Decreasing the distance can yield
         * higher quality shadows nearer to the viewer, as the shadow map resolution isn't being used up on far away
         * portions of the scene. In world units (meters).
         */
        float DirectionalShadowDistance = 250.0f;

        /**
         * Number of cascades to use for directional shadows. Higher number of cascades increases shadow quality as each
         * individual cascade has less area to cover, but can significantly increase performance cost, as well as a minor
         * increase in memory cost. Valid range is roughly [1, 6].
         */
        UINT32 NumCascades = 4;

        /**
         * Allows you to control how are directional shadow cascades distributed. Value of 1 means the cascades will be
         * linearly split, each cascade taking up the same amount of space. Value of 2 means each subsequent split will be
         * twice the size of the previous one (meaning cascades closer to the viewer cover a smaller area, and therefore
         * yield higher resolution shadows). Higher values increase the size disparity between near and far cascades at
         * an exponential rate. Valid range is roughly [1, 4].
         */
        float CascadeDistributionExponent = 3.0f;

        /**
         * Determines the number of samples used for percentage closer shadow map filtering. Higher values yield higher
         * quality shadows, at the cost of performance. Valid range is [1, 4].
         */
        UINT32 ShadowFilteringQuality = 4;
    };

    /** Settings that control temporal anti-aliasing. */
    struct TE_CORE_EXPORT TemporalAASettings
    {
        TemporalAASettings() = default;

        /** Enables or disables temporal anti-aliasing. */
        bool Enabled = false;

        /**
         * Number of different jittered positions to use. Each frame will use one position and subsequent frames
         * will use subsequent positions until this number of reached, at which point the positions start getting
         * re-used from the start.
         */
        UINT32 JitteredPositionCount = 8;

        /** Determines the distance between temporal AA samples. Larger values result in a sharper image. */
        float Sharpness = 1.0f;
    };

    /**
     * Settings that control the screen space reflections effect. Screen space reflections provide high quality mirror-like
     * reflections at low performance cost. They should be used together with reflection probes as the effects complement
     * each other. As the name implies, the reflections are only limited to geometry drawn on the screen and the system will
     * fall back to refl. probes when screen space data is unavailable. Similarly the system will fall back to refl. probes
     * for rougher (more glossy rather than mirror-like) surfaces. Those surfaces require a higher number of samples to
     * achieve the glossy look, so we instead fall back to refl. probes which are pre-filtered and can be quickly sampled.
     */
    struct TE_CORE_EXPORT ScreenSpaceReflectionsSettings
    {
        ScreenSpaceReflectionsSettings() = default;

        /** Enables or disables the SSR effect. */
        bool Enabled = true;

        /**
         * Quality of the SSR effect. Higher values cast more sample rays, and march those rays are lower increments for
         * better precision. This results in higher quality, as well as a higher performance requirement. Valid range is
         * [0, 4], default is 2.
         */
        UINT32 Quality = 2;

        /** Intensity of the screen space reflections. Valid range is [0, 1]. Default is 1 (100%). */
        float intensity = 1.0f;

        /**
         * Roughness at which screen space reflections start fading out and become replaced with refl. probes. Valid range
         * is [0, 1]. Default is 0.8.
         */
        float MaxRoughness = 0.8f;
    };

    /** RenderSettings struct */
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

        /** Parameters used for customizing screen space reflections. */
        ScreenSpaceReflectionsSettings ScreenSpaceReflections;

        /** Parameters used for customizing the bloom effect. */
        BloomSettings Bloom;

        /** Parameters used for customizing the motion blur effect. */
        MotionBlurSettings MotionBlur;

        /** Parameters used for customizing the gaussian depth of field effect. */
        DepthOfFieldSettings DepthOfField;

        /** Parameters used for customizing the temporal anti-aliasing effect. */
        TemporalAASettings TemporalAA;

        /** Parameters used for customizing shadow rendering. */
        ShadowsSettings ShadowSettings;

        /** Enables the fast approximate anti-aliasing effect. */
        bool EnableFXAA = true;

        /**
         * Log2 value to scale the eye adaptation by (for example 2^0 = 1). Smaller values yield darker image, while larger
         * yield brighter image. Allows you to customize exposure manually, applied on top of eye adaptation exposure (if
         * enabled). In range [-8, 8].
         */
        float ExposureScale = 1.0f;

        /**
         * Gamma value to adjust the image for. Larger values result in a brighter image. When tonemapping is turned
         * on the best gamma curve for the output device is chosen automatically and this value can by used to merely tweak
         * that curve. If tonemapping is turned off this is the exact value of the gamma curve that will be applied.
         */
        float Gamma = 2.2f;

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
         * It's possible to generate at each frame a dynamic cubemap environment texture. For each material, if
         * UseDynamicEnvironmentMap is set, renderer will try to find a lighprobe for this object and material. 
         * If not lightprobe has been found a cubemap will be generated and for each object using this material, this texture will
         * be bound on texture slot "EnvironmentMap"
         * Only renderables with UseForDynamicEnvMapping set to true will be use for rendering
         */
        bool EnableDynamicEnvMapping = false;

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
         * Add contrast to image output
         */
        float Contrast = 1.0f;

        /**
         * make image brighter
         */
        float Brightness = 0.0f;

        /**
         * The absolute base cull-distance for objects rendered through this camera in world units. Objects will use this
         * distance and apply their own factor to it to determine whether they should be visible.
         */
        float CullDistance = FLT_MAX;
    };
}
