#include "TeImGuiExt.h"

#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Math/TeRadian.h"
#include "Math/TeQuaternion.h"
#include "Renderer/TeCamera.h"
#include "Components/TeCCamera.h"
#include "Renderer/TeRenderSettings.h"
#include "Scene/TeTransform.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-security"  // warning: format string is not a string literal (potentially insecure)
#endif

namespace te
{
    bool ImGuiExt::RenderOptionFloat(float& value, const char* id, const char* text, 
        float min, float max, float width, bool disable)
    {
        const float previousValue = value;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::SliderFloat(text, &value, min, max, "%.2f");
        if (width > 0.0f) ImGui::PopItemWidth();
        ImGui::PopID();
        value = Math::Clamp(value, min, max);

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (previousValue != value)
            return true;

        return false;
    };

    bool ImGuiExt::RenderOptionBool(bool& value, const char* id, const char* text, bool disable)
    {
        const bool previousValue = value;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushID(id);
        ImGui::Checkbox(text, &value);
        ImGui::PopID();

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (previousValue != value)
            return true;

        return false;
    };

    bool ImGuiExt::RenderVector2(Vector2& vector, const char* id, const char* text, float width, bool disable)
    {
        bool hasChanged = false;
        auto& ctx = *ImGui::GetCurrentContext();

        float buttonsWidth = ImGui::CalcItemWidth() - width;
        float lineHeight = ctx.Font->FontSize + ctx.Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 20.0f, lineHeight };

        buttonsWidth = (buttonsWidth < 120.0f) ? 120.0f : buttonsWidth;

        if (fabs(vector.x) == 0.0f) vector.x = 0.0f;
        if (fabs(vector.y) == 0.0f) vector.y = 0.0f;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushID(text);
            ImGui::PushMultiItemsWidths(2, buttonsWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 5.0f });
                if (ImGui::Button("X", buttonSize)) {}

                ImGui::SameLine();
                if (ImGui::DragFloat("###X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f"))
                    hasChanged = true;
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button("Y", buttonSize)) {}

                ImGui::SameLine();
                if (ImGui::DragFloat("###Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f"))
                    hasChanged = true;
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::Text(text);
            ImGui::PopStyleVar();
        ImGui::PopID();

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        return hasChanged;
    };

    bool ImGuiExt::RenderVector3(Vector3& vector, const char* id, const char* text, float width, bool disable)
    {
        bool hasChanged = false;
        auto& ctx = *ImGui::GetCurrentContext();

        float buttonsWidth = ImGui::CalcItemWidth() - width;
        float lineHeight = ctx.Font->FontSize + ctx.Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 20.0f, lineHeight };

        buttonsWidth = (buttonsWidth < 120.0f) ? 120.0f : buttonsWidth;

        if (fabs(vector.x) == 0.0f) vector.x = 0.0f;
        if (fabs(vector.y) == 0.0f) vector.y = 0.0f;
        if (fabs(vector.z) == 0.0f) vector.z = 0.0f;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushID(text);
            ImGui::PushMultiItemsWidths(3, buttonsWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 5.0f });
                if (ImGui::Button("X", buttonSize)) {}

                ImGui::SameLine();
                if (ImGui::DragFloat("###X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f"))
                    hasChanged = true;
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button("Y", buttonSize)) {}

                ImGui::SameLine();
                if (ImGui::DragFloat("###Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f"))
                    hasChanged = true;
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button("Z", buttonSize)) {}

                ImGui::SameLine();
                if (ImGui::DragFloat("###Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f"))
                    hasChanged = true;
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::Text(text);
            ImGui::PopStyleVar();
        ImGui::PopID();

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        return hasChanged;
    };

    bool ImGuiExt::RenderTransform(Transform& transform, const char* text, bool disable)
    {
        bool hasChanged = false;

        Radian x, y, z;
        transform.GetRotation().ToEulerAngles(x, y, z);

        Vector3 position = transform.GetPosition();
        Vector3 rotation(x.ValueDegrees(), y.ValueDegrees(), z.ValueDegrees());
        Vector3 scale = transform.GetScale();

        ImGui::PushID(text);
            if (RenderVector3(position, "##transform_position_option", " Position", 20.0f, disable))
            {
                transform.SetPosition(position);
                hasChanged = true;
            }
            if (RenderVector3(rotation, "##transform_rotation_option", " Rotation", 20.0f, disable))
            {
                Quaternion rot;
                rot.FromEulerAngles(Radian(Degree(rotation.x)), Radian(Degree(rotation.y)), Radian(Degree(rotation.z)));
                transform.SetRotation(rot);
                hasChanged = true;
            }
            if (RenderVector3(scale, "##transform_scale_option", " Scale", 20.0f, disable))
            {
                transform.SetScale(scale);
                hasChanged = true;
            }

        ImGui::PopID();

        return hasChanged;
    };

    bool ImGuiExt::RenderColorRGBA(Vector4& color, const char* id, const char* text, float width,
        bool disable)
    {
        bool hasChanged = false;

        Vector4 oldColor = color;
        ImVec4 imColor = color;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushItemWidth(width);
        ImGui::ColorEdit4(id, (float*)&imColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text(text);
        ImGui::PopItemWidth();

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        color = Vector4(imColor.x, imColor.y, imColor.z, imColor.w);

        if (oldColor != color)
            hasChanged = true;

        return hasChanged;
    }

    bool ImGuiExt::RenderCameraGraphics(HCamera& camera, SPtr<RenderSettings> cameraSettings, float width)
    {
        bool hasChanged = false;

        // Render Output Type
        {
            static ImGuiExt::ComboOptions<int> outputTypeOptions;
            if (outputTypeOptions.Options.size() == 0)
            {
                outputTypeOptions.AddOption((int)RenderOutputType::Final, "Post processed");
                outputTypeOptions.AddOption((int)RenderOutputType::Color, "Raw");
                outputTypeOptions.AddOption((int)RenderOutputType::Velocity, "Velocity map");
                outputTypeOptions.AddOption((int)RenderOutputType::Emissive, "Emissive map");
                outputTypeOptions.AddOption((int)RenderOutputType::Depth, "Depth map");
                outputTypeOptions.AddOption((int)RenderOutputType::Normal, "Normal map");
                outputTypeOptions.AddOption((int)RenderOutputType::SSAO, "SSAO");
            }

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&cameraSettings->OutputType), "##output_type_option", "Output type", outputTypeOptions, width))
                hasChanged = true;
        }
        ImGui::Separator();

        // Projection type
        {
            static ImGuiExt::ComboOptions<int> projectionTypeOptions;
            if (projectionTypeOptions.Options.size() == 0)
            {
                projectionTypeOptions.AddOption((int)ProjectionType::PT_PERSPECTIVE, "Perspective");
                projectionTypeOptions.AddOption((int)ProjectionType::PT_ORTHOGRAPHIC, "Orthographic");
            }

            int projectionType = camera->GetProjectionType();
            int oldProjectionType = projectionType;

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&projectionType), "##projection_type_option", "Projection type", projectionTypeOptions, width))
                hasChanged = true;

            if (projectionType != oldProjectionType)
                camera->SetProjectionType((ProjectionType)projectionType);
        }
        ImGui::Separator();

        // Antialiasing
        {
            static ImGuiExt::ComboOptions<int> antialiasingOptions;
            if (antialiasingOptions.Options.size() == 0)
            {
                antialiasingOptions.AddOption((int)AntiAliasingAlgorithm::FXAA, "FXAA");
                antialiasingOptions.AddOption((int)AntiAliasingAlgorithm::TAA, "TAA");
                antialiasingOptions.AddOption((int)AntiAliasingAlgorithm::None, "None");
            }

            if (ImGuiExt::RenderOptionCombo<int>((int*)(&cameraSettings->AntialiasingAglorithm), "##aa_option", "Antialiasing", antialiasingOptions, width))
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
            Vector4 color = camera->GetViewport()->GetClearColorValue().GetAsVector4();

            if (ImGuiExt::RenderColorRGBA(color, "##clear_color_option", "Background color", width))
            {
                hasChanged = true;
                camera->GetViewport()->SetClearColorValue(Color(color));
                // We need a more violent way to refresh camera (if skybox is null)
                camera->GetInternal()->MarkCoreDirty();
            }
        }

        {
            Vector4 color = camera->GetRenderSettings()->SceneLightColor.GetAsVector4();

            if (ImGuiExt::RenderColorRGBA(color, "##scene_light_color_option", "Scene light color", width))
            {
                hasChanged = true;
                camera->GetRenderSettings()->SceneLightColor = Color(color);
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

        return hasChanged;
    }

    bool ImGuiExt::RenderCameraPostProcessing(HCamera& camera, SPtr<RenderSettings> cameraSettings, float width)
    {
        bool hasChanged = false;

        // HDR, ToneMapping
        {
            if (ImGuiExt::RenderOptionBool(cameraSettings->EnableHDR, "##hdr_option", "Enable HDR"))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionBool(cameraSettings->Tonemapping.Enabled, "##tonemapping_option", "Enable Tonemapping"))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionFloat(cameraSettings->ExposureScale, "##exposure_option", "Exposure", 0.0f, 5.0f, width))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionFloat(cameraSettings->Gamma, "##gamma_option", "Gamma", 0.0f, 5.0f, width))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionFloat(cameraSettings->Contrast, "##contrast_option", "Contrast", 0.0f, 5.0f, width))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionFloat(cameraSettings->Brightness, "##brightness_option", "Brightness", -2.0f, 2.0f, width))
                hasChanged = true;
            ImGui::Separator();
            if (ImGuiExt::RenderOptionFloat(cameraSettings->CullDistance, "##cull_distance_option", "Cull distance", 0.0f, 10000.0f, width))
                hasChanged = true;
        }
        ImGui::Separator();

        // Blur
        {
            if (ImGuiExt::RenderOptionBool(cameraSettings->MotionBlur.Enabled, "##blur_option", "Enable blur"))
                hasChanged = true;

            if (cameraSettings->MotionBlur.Enabled)
            {
                static ImGuiExt::ComboOptions<int> blurQualityOptions;
                if (blurQualityOptions.Options.size() == 0)
                {
                    blurQualityOptions.AddOption((int)MotionBlurQuality::Ultra, "Ultra");
                    blurQualityOptions.AddOption((int)MotionBlurQuality::High, "High");
                    blurQualityOptions.AddOption((int)MotionBlurQuality::Medium, "Medium");
                    blurQualityOptions.AddOption((int)MotionBlurQuality::Low, "Low");
                    blurQualityOptions.AddOption((int)MotionBlurQuality::VeryLow, "Very low");
                }

                if (ImGuiExt::RenderOptionCombo<int>((int*)(&cameraSettings->MotionBlur.Quality), "##blur_quality_option", "Quality", blurQualityOptions, width))
                    hasChanged = true;
            }
        }
        ImGui::Separator();

        // Bloom
        {
            if (ImGuiExt::RenderOptionBool(cameraSettings->Bloom.Enabled, "##bloom_option", "Enable bloom"))
                hasChanged = true;

            if (cameraSettings->Bloom.Enabled)
            {
                if (ImGuiExt::RenderOptionFloat(cameraSettings->Bloom.Intensity, "##bloom_intensity_option", "Intensity", 0.0f, 5.0f, width))
                    hasChanged = true;

                static ImGuiExt::ComboOptions<int> bloomQualityOptions;
                if (bloomQualityOptions.Options.size() == 0)
                {
                    bloomQualityOptions.AddOption((int)BloomQuality::High, "High");
                    bloomQualityOptions.AddOption((int)BloomQuality::Medium, "Medium");
                    bloomQualityOptions.AddOption((int)BloomQuality::Low, "Low");
                }

                if (ImGuiExt::RenderOptionCombo<int>((int*)(&cameraSettings->Bloom.Quality), "##bloom_quality_option", "Quality", bloomQualityOptions, width))
                    hasChanged = true;
            }
        }

        return hasChanged;
    }

    bool ImGuiExt::RenderOptionComboComponent(HCamera* value, const char* id, const char* text, ComboOptions<HCamera>& options,
        float width)
    {
        if (!Math::ApproxEquals(width, 0.0f) && width < 75.0f)
            width = 75.0f;

        bool hasChanged = false;
        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);

        if (ImGui::BeginCombo(text, options[*value].Key->GetName().c_str()))
        {
            for (const auto& option : options.Options)
            {
                const bool isSelected = (*value == option.Key);
                ImGui::PushID(option.Key->GetUUID().ToString().c_str());
                if (ImGui::Selectable(option.Key->GetName().c_str(), isSelected))
                {
                    *value = option.Key;
                    hasChanged = true;
                }
                ImGui::PopID();

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::PopID();

        return hasChanged;
    };

    void ImGuiExt::RenderImage(SPtr<Texture> texture, UINT32 maxMip, const Vector2& size, const Vector2& offset)
    {
        if (!texture || texture->GetProperties().GetTextureType() != TextureType::TEX_TYPE_2D)
            return;

        UINT32 mipMap = texture->GetProperties().GetNumMipmaps();
        if (mipMap > maxMip) mipMap = maxMip;

        SPtr<TextureView> textureView = texture->RequestView(
            0, mipMap, 0, texture->GetProperties().GetNumFaces(),
            GVU_DEFAULT
        );

        void* rawData = textureView->GetRawData();

        ImVec2 cursor = ImGui::GetCursorPos();
        cursor.x += offset.x;
        cursor.y += offset.y;
        ImGui::SetCursorPos(cursor);

        ImGui::Image(
            static_cast<ImTextureID>(rawData),
            ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
            ImVec2(0, 0)
        );

        cursor.x -= offset.x;
        cursor.x -= offset.y;
        ImGui::SetCursorPos(cursor);
    }

    template<typename T>
    bool ImGuiExt::RenderOptionInt(T& value, const char* id, const char* text,
        T min, T max, float width, bool disable)
    {
        const int previousValue = static_cast<int>(value);;
        int newValue = previousValue;

        if (disable)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::SliderInt(text, &newValue, min, max, "%.d");
        if (width > 0.0f) ImGui::PopItemWidth();
        ImGui::PopID();
        value = newValue;
        value = Math::Clamp(value, min, max);

        if (disable)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        if (previousValue != newValue)
            return true;

        return false;
    };

    template bool ImGuiExt::RenderOptionInt(int& value, const char* id, const char* text,
        int min, int max, float width, bool disable);
    template bool ImGuiExt::RenderOptionInt(UINT32& value, const char* id, const char* text,
        UINT32 min, UINT32 max, float width, bool disable);
}
