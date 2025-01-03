#include "Renderer/TeRenderSettings.h"

namespace te
{
    void RenderSettings::ExportJson(nlohmann::json& document) const
    {
        document = {
            {
                "autoExposure", {
                    { "enabled", AutoExposure.Enabled},
                    { "histogramLog2Min", AutoExposure.HistogramLog2Min },
                    { "histogramLog2Max", AutoExposure.HistogramLog2Max },
                    { "histogramPctLow", AutoExposure.HistogramPctLow },
                    { "histogramPctHigh", AutoExposure.HistogramPctHigh },
                    { "minEyeAdaptation", AutoExposure.MinEyeAdaptation },
                    { "maxEyeAdaptation", AutoExposure.MaxEyeAdaptation },
                    { "eyeAdaptationSpeedUp", AutoExposure.EyeAdaptationSpeedUp },
                    { "eyeAdaptationSpeedDown", AutoExposure.EyeAdaptationSpeedDown }
                }
            },
            {
                "tonemapping", {
                    { "enabled", Tonemapping.Enabled }
                }
            },
            {
                "ambientOcclusion", {
                    { "ambienOcclusion", AmbientOcclusion.Enabled },
                    { "radius", AmbientOcclusion.Radius },
                    { "bias", AmbientOcclusion.Bias },
                    { "fadeDistance", AmbientOcclusion.FadeDistance },
                    { "fadeRange", AmbientOcclusion.FadeRange },
                    { "intensity", AmbientOcclusion.Intensity },
                    { "power", AmbientOcclusion.Power },
                    { "quality", AmbientOcclusion.Quality }
                }
            },
            {
                "screenSpaceReflections", {
                    { "enabled", ScreenSpaceReflections.Enabled },
                    { "quality", ScreenSpaceReflections.Quality },
                    { "intensity", ScreenSpaceReflections.Intensity },
                    { "maxRoughness", ScreenSpaceReflections.MaxRoughness}
                }
            },
            {
                "bloom", {
                    { "enabled", Bloom.Enabled },
                    { "intensity", Bloom.Intensity },
                    { "quality", Bloom.Quality },
                    { "filterSize", Bloom.FilterSize },
                    { "maxBlurSamples", Bloom.MaxBlurSamples }
                }
            },
            {
                "MotionBlur", {
                    { "enabled", MotionBlur.Enabled },
                    { "domain", MotionBlur.Domain }
                }
            },
            {
                "DepthOfField", {
                    { "enabled", DepthOfField.Enabled },
                    { "type", DepthOfField.type },
                    { "focalDistance", DepthOfField.FocalDistance },
                    { "nearTransitionRange", DepthOfField.NearTransitionRange },
                    { "farTransitionRange", DepthOfField.FarTransitionRange },
                    { "maxBokehSize", DepthOfField.MaxBokehSize },
                    { "adaptativeColorThreshold", DepthOfField.AdaptiveColorThreshold },
                    { "adaptiveRadiusThreshold", DepthOfField.AdaptiveRadiusThreshold }
                }
            },
            {
                "TemporalAA", {
                    { "JitteredPositionCount", TemporalAA.JitteredPositionCount },
                    { "sharpness", TemporalAA.Sharpness }
                }
            },
            {
                "Shadows", {
                    { "enabled", ShadowSettings.Enabled },
                    { "directionalShadowDistance", ShadowSettings.DirectionalShadowDistance },
                    { "numCascades", ShadowSettings.NumCascades },
                    { "cascadeDistributionExponent", ShadowSettings.CascadeDistributionExponent },
                    { "ShadowFilteringQuality", ShadowSettings.ShadowFilteringQuality }
                }
            },
            { "outputType", OutputType },
            { "antialiasingAglorithm", AntialiasingAglorithm },
            { "exposureScale", ExposureScale },
            { "gamma", Gamma },
            { "enableHDR", EnableHDR },
            { "enableLighting", EnableLighting },
            { "enableDynamicEnvMapping", EnableDynamicEnvMapping },
            { "overlayOnly", OverlayOnly },
            { "enableSkybox", EnableSkybox },
            { "Contrast", Contrast },
            { "brightness", Brightness },
            { "cullDistance", CullDistance },
            { "useGlobalIllumination", UseGlobalIllumination },
            { "useZPrepass", UseZPrepass }
        };

        Bloom.Tint.ExportJson(document["bloom"]["tin"]);
        SceneLightColor.ExportJson(document["sceneLightColor"]);
    }
}
