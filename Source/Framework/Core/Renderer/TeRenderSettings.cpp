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
                "motionBlur", {
                    { "enabled", MotionBlur.Enabled },
                    { "domain", MotionBlur.Domain }
                }
            },
            {
                "depthOfField", {
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
                "temporalAA", {
                    { "JitteredPositionCount", TemporalAA.JitteredPositionCount },
                    { "sharpness", TemporalAA.Sharpness }
                }
            },
            {
                "shadows", {
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
            { "contrast", Contrast },
            { "brightness", Brightness },
            { "cullDistance", CullDistance },
            { "useGlobalIllumination", UseGlobalIllumination },
            { "useZPrepass", UseZPrepass }
        };

        Bloom.Tint.ExportJson(document["bloom"]["tin"]);
        SceneLightColor.ExportJson(document["sceneLightColor"]);
    }

    void RenderSettings::ImportJson(const nlohmann::json& document, RenderSettings& renderSettings)
    {
        // AutoExposure
        if (document.contains("autoExposure"))
        {
            const auto& ae = document["autoExposure"];
            if (ae.contains("enabled")) renderSettings.AutoExposure.Enabled = ae["enabled"].get<bool>();
            if (ae.contains("histogramLog2Min")) renderSettings.AutoExposure.HistogramLog2Min = ae["histogramLog2Min"].get<float>();
            if (ae.contains("histogramLog2Max")) renderSettings.AutoExposure.HistogramLog2Max = ae["histogramLog2Max"].get<float>();
            if (ae.contains("histogramPctLow")) renderSettings.AutoExposure.HistogramPctLow = ae["histogramPctLow"].get<float>();
            if (ae.contains("histogramPctHigh")) renderSettings.AutoExposure.HistogramPctHigh = ae["histogramPctHigh"].get<float>();
            if (ae.contains("minEyeAdaptation")) renderSettings.AutoExposure.MinEyeAdaptation = ae["minEyeAdaptation"].get<float>();
            if (ae.contains("maxEyeAdaptation")) renderSettings.AutoExposure.MaxEyeAdaptation = ae["maxEyeAdaptation"].get<float>();
            if (ae.contains("eyeAdaptationSpeedUp")) renderSettings.AutoExposure.EyeAdaptationSpeedUp = ae["eyeAdaptationSpeedUp"].get<float>();
            if (ae.contains("eyeAdaptationSpeedDown")) renderSettings.AutoExposure.EyeAdaptationSpeedDown = ae["eyeAdaptationSpeedDown"].get<float>();
        }

        // Tonemapping
        if (document.contains("tonemapping"))
        {
            const auto& tm = document["tonemapping"];
            if (tm.contains("enabled")) renderSettings.Tonemapping.Enabled = tm["enabled"].get<bool>();
        }

        // AmbientOcclusion
        if (document.contains("ambientOcclusion"))
        {
            const auto& ao = document["ambientOcclusion"];
            if (ao.contains("ambienOcclusion")) renderSettings.AmbientOcclusion.Enabled = ao["ambienOcclusion"].get<bool>();
            if (ao.contains("radius")) renderSettings.AmbientOcclusion.Radius = ao["radius"].get<float>();
            if (ao.contains("bias")) renderSettings.AmbientOcclusion.Bias = ao["bias"].get<float>();
            if (ao.contains("fadeDistance")) renderSettings.AmbientOcclusion.FadeDistance = ao["fadeDistance"].get<float>();
            if (ao.contains("fadeRange")) renderSettings.AmbientOcclusion.FadeRange = ao["fadeRange"].get<float>();
            if (ao.contains("intensity")) renderSettings.AmbientOcclusion.Intensity = ao["intensity"].get<float>();
            if (ao.contains("power")) renderSettings.AmbientOcclusion.Power = ao["power"].get<float>();
            if (ao.contains("quality")) renderSettings.AmbientOcclusion.Quality = static_cast<AmbientOcclusionQuality>(ao["quality"].get<int>());
        }

        // ScreenSpaceReflections
        if (document.contains("screenSpaceReflections"))
        {
            const auto& ssr = document["screenSpaceReflections"];
            if (ssr.contains("enabled")) renderSettings.ScreenSpaceReflections.Enabled = ssr["enabled"].get<bool>();
            if (ssr.contains("quality")) renderSettings.ScreenSpaceReflections.Quality = ssr["quality"].get<uint32_t>();
            if (ssr.contains("intensity")) renderSettings.ScreenSpaceReflections.Intensity = ssr["intensity"].get<float>();
            if (ssr.contains("maxRoughness")) renderSettings.ScreenSpaceReflections.MaxRoughness = ssr["maxRoughness"].get<float>();
        }

        // Bloom
        if (document.contains("bloom"))
        {
            const auto& bloom = document["bloom"];
            if (bloom.contains("enabled")) renderSettings.Bloom.Enabled = bloom["enabled"].get<bool>();
            if (bloom.contains("intensity")) renderSettings.Bloom.Intensity = bloom["intensity"].get<float>();
            if (bloom.contains("quality")) renderSettings.Bloom.Quality = static_cast<BloomQuality>(bloom["quality"].get<int>());
            if (bloom.contains("filterSize")) renderSettings.Bloom.FilterSize = bloom["filterSize"].get<float>();
            if (bloom.contains("maxBlurSamples")) renderSettings.Bloom.MaxBlurSamples = bloom["maxBlurSamples"].get<uint32_t>();
            if (bloom.contains("tin")) renderSettings.Bloom.Tint = Color::ImportJson(bloom["tin"]);
        }

        // MotionBlur
        if (document.contains("motionBlur"))
        {
            const auto& mb = document["motionBlur"];
            if (mb.contains("enabled")) renderSettings.MotionBlur.Enabled = mb["enabled"].get<bool>();
            if (mb.contains("domain")) renderSettings.MotionBlur.Domain = static_cast<MotionBlurDomain>(mb["domain"].get<int>());
            if (mb.contains("quality")) renderSettings.MotionBlur.Quality = static_cast<MotionBlurQuality>(mb["quality"].get<int>());
        }

        // DepthOfField
        if (document.contains("depthOfField"))
        {
            const auto& dof = document["depthOfField"];
            if (dof.contains("enabled")) renderSettings.DepthOfField.Enabled = dof["enabled"].get<bool>();
            if (dof.contains("type")) renderSettings.DepthOfField.type = static_cast<DepthOfFieldType>(dof["type"].get<int>());
            if (dof.contains("focalDistance")) renderSettings.DepthOfField.FocalDistance = dof["focalDistance"].get<float>();
            if (dof.contains("nearTransitionRange")) renderSettings.DepthOfField.NearTransitionRange = dof["nearTransitionRange"].get<float>();
            if (dof.contains("farTransitionRange")) renderSettings.DepthOfField.FarTransitionRange = dof["farTransitionRange"].get<float>();
            if (dof.contains("maxBokehSize")) renderSettings.DepthOfField.MaxBokehSize = dof["maxBokehSize"].get<float>();
            if (dof.contains("adaptativeColorThreshold")) renderSettings.DepthOfField.AdaptiveColorThreshold = dof["adaptativeColorThreshold"].get<float>();
            if (dof.contains("adaptiveRadiusThreshold")) renderSettings.DepthOfField.AdaptiveRadiusThreshold = dof["adaptiveRadiusThreshold"].get<float>();
        }

        // TemporalAA
        if (document.contains("temporalAA"))
        {
            const auto& taa = document["temporalAA"];
            if (taa.contains("JitteredPositionCount")) renderSettings.TemporalAA.JitteredPositionCount = taa["JitteredPositionCount"].get<uint32_t>();
            if (taa.contains("sharpness")) renderSettings.TemporalAA.Sharpness = taa["sharpness"].get<float>();
        }

        // Shadows
        if (document.contains("shadows"))
        {
            const auto& sh = document["shadows"];
            if (sh.contains("enabled")) renderSettings.ShadowSettings.Enabled = sh["enabled"].get<bool>();
            if (sh.contains("directionalShadowDistance")) renderSettings.ShadowSettings.DirectionalShadowDistance = sh["directionalShadowDistance"].get<float>();
            if (sh.contains("numCascades")) renderSettings.ShadowSettings.NumCascades = sh["numCascades"].get<uint32_t>();
            if (sh.contains("cascadeDistributionExponent")) renderSettings.ShadowSettings.CascadeDistributionExponent = sh["cascadeDistributionExponent"].get<float>();
            if (sh.contains("ShadowFilteringQuality")) renderSettings.ShadowSettings.ShadowFilteringQuality = sh["ShadowFilteringQuality"].get<uint32_t>();
        }

        // Simple values
        if (document.contains("outputType")) renderSettings.OutputType = static_cast<RenderOutputType>(document["outputType"].get<int>());
        if (document.contains("antialiasingAglorithm")) renderSettings.AntialiasingAglorithm = static_cast<AntiAliasingAlgorithm>(document["antialiasingAglorithm"].get<int>());
        if (document.contains("exposureScale")) renderSettings.ExposureScale = document["exposureScale"].get<float>();
        if (document.contains("gamma")) renderSettings.Gamma = document["gamma"].get<float>();
        if (document.contains("enableHDR")) renderSettings.EnableHDR = document["enableHDR"].get<bool>();
        if (document.contains("enableLighting")) renderSettings.EnableLighting = document["enableLighting"].get<bool>();
        if (document.contains("enableDynamicEnvMapping")) renderSettings.EnableDynamicEnvMapping = document["enableDynamicEnvMapping"].get<bool>();
        if (document.contains("overlayOnly")) renderSettings.OverlayOnly = document["overlayOnly"].get<bool>();
        if (document.contains("enableSkybox")) renderSettings.EnableSkybox = document["enableSkybox"].get<bool>();
        if (document.contains("contrast")) renderSettings.Contrast = document["contrast"].get<float>();
        if (document.contains("brightness")) renderSettings.Brightness = document["brightness"].get<float>();
        if (document.contains("cullDistance")) renderSettings.CullDistance = document["cullDistance"].get<float>();
        if (document.contains("useGlobalIllumination")) renderSettings.UseGlobalIllumination = document["useGlobalIllumination"].get<bool>();
        if (document.contains("useZPrepass")) renderSettings.UseZPrepass = document["useZPrepass"].get<bool>();

        // SceneLightColor
        if (document.contains("sceneLightColor"))
            renderSettings.SceneLightColor = Color::ImportJson(document["sceneLightColor"]);
    }
}
