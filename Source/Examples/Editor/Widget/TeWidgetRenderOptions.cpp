#include "TeWidgetRenderOptions.h"
#include "TeWidgetViewport.h"

#include "Renderer/TeRenderer.h"
#include "Manager/TeRendererManager.h"
#include "Scene/TeSceneManager.h"

#include "Renderer/TeRenderSettings.h"
#include "Renderer/TeCamera.h"

#include "Components/TeCCamera.h"

#include "../TeEditor.h"

namespace te
{
    WidgetRenderOptions::WidgetRenderOptions()
        : Widget(WidgetType::RenderOptions)
    {
        _title = RENDER_OPTIONS_TITLE;
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    {
        bool hasChanged = false;
        bool anotherCameraSelected = false;

        HCamera& camera = Editor::Instance().GetViewportCamera();
        auto cameraSettings = camera->GetRenderSettings();

        const auto renderOptionFloat = [&](
            float& value,
            const char* id, 
            const char* text,
            float min = 0.0f, 
            float max = std::numeric_limits<float>::max())
        {
            const float previousValue = value;

            ImGui::PushID(id);
            ImGui::SliderFloat(text, &value, min, max, "%.2f");
            ImGui::PopID();
            value = Math::Clamp(value, min, max);

            if (previousValue != value)
                hasChanged = true;
        };

        const auto renderOptionInt = [&](
            int& value,
            const char* id,
            const char* text,
            int min = 0,
            int max = std::numeric_limits<int>::max())
        {
            const int previousValue = value;

            ImGui::PushID(id);
            ImGui::SliderInt(text, &value, min, max, "%.d");
            ImGui::PopID();
            value = Math::Clamp(value, min, max);

            if (previousValue != value)
                hasChanged = true;
        };

        const auto renderOptionBool = [&](
            bool& value,
            const char* id,
            const char* text
            )
        {
            const bool previousValue = value;

            ImGui::PushID(id);
            ImGui::Checkbox(text, &value);
            ImGui::PopID();

            if (previousValue != value)
                hasChanged = true;
        };

        const auto renderOptionCombo = [&](
            int* value,
            const char* id,
            const char* text,
            Vector<int>& options,
            Vector<String>& labels
            )
        {
            ImGui::PushID(id);
            if (ImGui::BeginCombo(text, labels[*value].c_str()))
            {
                for (int i = 0; i < options.size(); i++)
                {
                    const bool isSelected = (*value == options[i]);
                    if (ImGui::Selectable(labels[i].c_str(), isSelected))
                    {
                        *value = options[i];
                        hasChanged = true;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        };

        if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Render Output Type
            {
                Vector<int> outputTypeOptions = {
                    (int)RenderOutputType::Final,
                    (int)RenderOutputType::Color,
                    (int)RenderOutputType::Velocity,
                    (int)RenderOutputType::Emissive,
                    (int)RenderOutputType::Depth,
                    (int)RenderOutputType::Normal
                };

                Vector<String> outputTypeLabels = {
                    "Final", "Color", "Depth", "Emissive", "Velocity", "Normal"
                };

                renderOptionCombo((int*)(&cameraSettings->OutputType), "##output_type_option", "Output type",
                    outputTypeOptions, outputTypeLabels);
            }
            ImGui::Separator();

            // Projection type
            {
                Vector<int> projectionTypeOptions = {
                    (int)ProjectionType::PT_ORTHOGRAPHIC,
                    (int)ProjectionType::PT_PERSPECTIVE
                    
                };

                Vector<String> projectionTypeLabels = {
                    "Orthographic", "Perspective"
                };

                int projectionType = camera->GetProjectionType();
                int oldProjectionType = projectionType;

                renderOptionCombo((int*)(&projectionType), "##projection_type_option", "Projection type",
                    projectionTypeOptions, projectionTypeLabels);

                if (projectionType != oldProjectionType)
                    camera->SetProjectionType((ProjectionType)projectionType);
            }
            ImGui::Separator();
            
            // Antialiasing
            {
                Vector<int> antialiasingOptions = {
                    (int)AntiAliasingAlgorithm::FXAA,
                    (int)AntiAliasingAlgorithm::TAA,
                    (int)AntiAliasingAlgorithm::None
                };

                Vector<String> antialiasingLabels = {
                    "FXAA", "TAA", "None"
                };

                renderOptionCombo((int*)(&cameraSettings->AntialiasingAglorithm), "##aa_option", "Antialiasing", 
                    antialiasingOptions, antialiasingLabels);

                if (cameraSettings->AntialiasingAglorithm == AntiAliasingAlgorithm::TAA)
                {
                    renderOptionInt((int&)(cameraSettings->TemporalAA.JitteredPositionCount), "##taa_jittered_option", "Jittered positions", 1, 8);
                    renderOptionFloat((cameraSettings->TemporalAA.Sharpness), "##taa_sharpness_option", "Sharpness", 0.1f, 16.0f);
                }
            }
            ImGui::Separator();

            // FOV
            {
                float fov = camera->GetHorzFOV().ValueDegrees();
                float oldFov = fov;
                renderOptionFloat(fov, "##fov_option", "FOV", 1.0f, 179.0f);

                if (fov != oldFov)
                    camera->SetHorzFOV(Radian(Degree(fov)));
            }
            ImGui::Separator();

            // Lighting, Shadowing
            {
                renderOptionBool(cameraSettings->EnableLighting, "##lighting_option", "Enable lighting");
                renderOptionBool(cameraSettings->EnableShadows, "##shadows_option", "Enable shadows");
            }
            ImGui::Separator();

            // Skybox
            {
                renderOptionBool(cameraSettings->EnableSkybox, "##skybox_option", "Enable skybox");
            }
            ImGui::Separator();

            // Overlay Only
            {
                renderOptionBool(cameraSettings->OverlayOnly, "##overlay_option", "Overlay only");
            }
            ImGui::Separator();

            // RenderTarget clear color value
            {
                Vector4 oldColor = camera->GetViewport()->GetClearColorValue().GetAsVector4();
                ImVec4 imColor = camera->GetViewport()->GetClearColorValue().GetAsVector4();
                
                ImGui::ColorEdit4("##clear_color_option", (float*)&imColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine();
                ImGui::Text("Background color");
                
                Vector4 newColor = Vector4(imColor.x, imColor.y, imColor.z, imColor.w);

                if (oldColor != newColor)
                {
                    hasChanged = true;
                    camera->GetViewport()->SetClearColorValue(Color(newColor));
                }
            }
            ImGui::Separator();

            // near and far cleaping plane
            {
                float near = camera->GetNearClipDistance();
                float far = camera->GetFarClipDistance();
                float oldNear = near, oldFar = far;

                ImGui::DragFloatRange2("Clip distance", &near, &far, 0.1f, 0.1f, 2500.0f, "Near: %.1f", "Far: %.1f", ImGuiSliderFlags_AlwaysClamp);

                if (near != oldNear)
                {
                    camera->SetNearClipDistance(near);
                    hasChanged = true;
                }
                if (far != oldFar)
                {
                    camera->SetFarClipDistance(far);
                    hasChanged = true;
                }
            }
        }

        if (ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // HDR, ToneMapping
            {
                renderOptionBool(cameraSettings->EnableHDR, "##hdr_option", "Enable HDR");
                renderOptionBool(cameraSettings->Tonemapping.Enabled, "##tonemapping_option", "Enable Tonemapping");
                renderOptionFloat(cameraSettings->ExposureScale, "##exposure_option", "Exposure", 0.0f, 5.0f);
                renderOptionFloat(cameraSettings->Gamma, "##gamma_option", "Gamma", 0.0f, 5.0f);
                renderOptionFloat(cameraSettings->Contrast, "##contrast_option", "Contrast", 0.0f, 5.0f);
                renderOptionFloat(cameraSettings->Brightness, "##brightness_option", "Brightness", -2.0f, 2.0f);
            }
            ImGui::Separator();

            // Blur
            {
                renderOptionBool(cameraSettings->MotionBlur.Enabled, "##blur_option", "Enable blur");

                if (cameraSettings->MotionBlur.Enabled)
                {
                    Vector<int> blurQualityOptions = {
                        (int)MotionBlurQuality::VeryLow,
                        (int)MotionBlurQuality::Low,
                        (int)MotionBlurQuality::Medium,
                        (int)MotionBlurQuality::High,
                        (int)MotionBlurQuality::Ultra
                    };

                    Vector<String> blurQualityLabels = {
                        "Very low", "Low", "Medium", "High", "Ultra"
                    };

                    renderOptionCombo((int*)(&cameraSettings->MotionBlur.Quality), "##blur_quality_option", "Quality",
                        blurQualityOptions, blurQualityLabels);
                }
            }
            ImGui::Separator();

            // Bloom
            {
                renderOptionBool(cameraSettings->Bloom.Enabled, "##bloom_option", "Enable bloom");

                if (cameraSettings->Bloom.Enabled)
                {
                    renderOptionFloat(cameraSettings->Bloom.Intensity, "##bloom_intensity_option", "Intensity", 0.0f, 5.0f);
                }
            }
        }

        if (hasChanged)
        {
            Editor::Instance().NeedsRedraw();
            camera->SetRenderSettings(cameraSettings);
        }
    }

    void WidgetRenderOptions::UpdateBackground()
    { }
}
