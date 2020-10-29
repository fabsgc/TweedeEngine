#include "TeWidgetRenderOptions.h"
#include "TeWidgetViewport.h"

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
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetRenderOptions::~WidgetRenderOptions()
    { }

    void WidgetRenderOptions::Initialize()
    { }

    void WidgetRenderOptions::Update()
    {
        bool hasChanged = false;
        HCamera& camera = Editor::Instance().GetViewportCamera();
        auto cameraSettings = camera->GetRenderSettings();
        const float width = ImGui::GetWindowContentRegionWidth() - 100.0f;

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
                    "Final", "Color", "Velocity", "Emissive", "Depth", "Normal"
                };

                if (ImGuiExt::RenderOptionCombo((int*)(&cameraSettings->OutputType), "##output_type_option", "Output type", outputTypeOptions, outputTypeLabels, width))
                    hasChanged = true;
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

                if(ImGuiExt::RenderOptionCombo((int*)(&projectionType), "##projection_type_option", "Projection type", projectionTypeOptions, projectionTypeLabels, width))
                    hasChanged = true;

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

                if (ImGuiExt::RenderOptionCombo((int*)(&cameraSettings->AntialiasingAglorithm), "##aa_option", "Antialiasing", antialiasingOptions, antialiasingLabels, width))
                    hasChanged = true;

                if (cameraSettings->AntialiasingAglorithm == AntiAliasingAlgorithm::TAA)
                {
                    if (ImGuiExt::RenderOptionInt((int&)(cameraSettings->TemporalAA.JitteredPositionCount), "##taa_jittered_option", "Jittered positions", 1, 8, width))
                        hasChanged = true;
                    if (ImGuiExt::RenderOptionFloat((cameraSettings->TemporalAA.Sharpness), "##taa_sharpness_option", "Sharpness", 0.1f, 16.0f, width))
                        hasChanged = true;
                }
            }
            ImGui::Separator();

            // FOV
            {
                float fov = camera->GetHorzFOV().ValueDegrees();
                float oldFov = fov;

                if (ImGuiExt::RenderOptionFloat(fov, "##fov_option", "FOV", 1.0f, 179.0f, width))
                    hasChanged = true;

                if (fov != oldFov)
                    camera->SetHorzFOV(Radian(Degree(fov)));
            }
            ImGui::Separator();

            // Lighting, Shadowing
            {
                if (ImGuiExt::RenderOptionBool(cameraSettings->EnableLighting, "##lighting_option", "Enable lighting"))
                    hasChanged = true;
                
                if (ImGuiExt::RenderOptionBool(cameraSettings->EnableShadows, "##shadows_option", "Enable shadows"))
                    hasChanged = true;
            }
            ImGui::Separator();

            // Skybox
            {
                if (ImGuiExt::RenderOptionBool(cameraSettings->EnableSkybox, "##skybox_option", "Enable skybox"))
                    hasChanged = true;
            }
            ImGui::Separator();

            // Overlay Only
            {
                if (ImGuiExt::RenderOptionBool(cameraSettings->OverlayOnly, "##overlay_option", "Overlay only"))
                    hasChanged = true;
            }
            ImGui::Separator();

            // RenderTarget clear color value
            {
                Vector4 oldColor = camera->GetViewport()->GetClearColorValue().GetAsVector4();
                ImVec4 imColor = camera->GetViewport()->GetClearColorValue().GetAsVector4();
                
                ImGui::PushItemWidth(width);
                ImGui::ColorEdit4("##clear_color_option", (float*)&imColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine();
                ImGui::Text("Background color");
                ImGui::PopItemWidth();
                
                Vector4 newColor = Vector4(imColor.x, imColor.y, imColor.z, imColor.w);

                if (oldColor != newColor)
                {
                    hasChanged = true;
                    camera->GetViewport()->SetClearColorValue(Color(newColor));

                    // We need a more violent way to refresh camera (if skybox is null)
                    camera->_getCamera()->MarkCoreDirty();
                }
            }
            ImGui::Separator();

            // near and far cleaping plane
            {
                float near = camera->GetNearClipDistance();
                float far = camera->GetFarClipDistance();
                float oldNear = near, oldFar = far;

                ImGui::PushItemWidth(width);
                ImGui::DragFloatRange2("Clip distance", &near, &far, 0.1f, 0.1f, 2500.0f, "Near: %.1f", "Far: %.1f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::PopItemWidth();

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
                if (ImGuiExt::RenderOptionBool(cameraSettings->EnableHDR, "##hdr_option", "Enable HDR"))
                    hasChanged = true;
                if (ImGuiExt::RenderOptionBool(cameraSettings->Tonemapping.Enabled, "##tonemapping_option", "Enable Tonemapping"))
                    hasChanged = true;
                if (ImGuiExt::RenderOptionFloat(cameraSettings->ExposureScale, "##exposure_option", "Exposure", 0.0f, 5.0f, width))
                    hasChanged = true;
                if (ImGuiExt::RenderOptionFloat(cameraSettings->Gamma, "##gamma_option", "Gamma", 0.0f, 5.0f, width))
                    hasChanged = true;
                if (ImGuiExt::RenderOptionFloat(cameraSettings->Contrast, "##contrast_option", "Contrast", 0.0f, 5.0f, width))
                    hasChanged = true;
                if (ImGuiExt::RenderOptionFloat(cameraSettings->Brightness, "##brightness_option", "Brightness", -2.0f, 2.0f, width))
                    hasChanged = true;
            }
            ImGui::Separator();

            // Blur
            {
                if(ImGuiExt::RenderOptionBool(cameraSettings->MotionBlur.Enabled, "##blur_option", "Enable blur"))
                    hasChanged = true;

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

                    if(ImGuiExt::RenderOptionCombo((int*)(&cameraSettings->MotionBlur.Quality), "##blur_quality_option", "Quality", blurQualityOptions, blurQualityLabels, width))
                        hasChanged = true;
                }
            }
            ImGui::Separator();

            // Bloom
            {
                if(ImGuiExt::RenderOptionBool(cameraSettings->Bloom.Enabled, "##bloom_option", "Enable bloom"))
                    hasChanged = true;

                if (cameraSettings->Bloom.Enabled)
                {
                    if(ImGuiExt::RenderOptionFloat(cameraSettings->Bloom.Intensity, "##bloom_intensity_option", "Intensity", 0.0f, 5.0f, width))
                        hasChanged = true;
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
