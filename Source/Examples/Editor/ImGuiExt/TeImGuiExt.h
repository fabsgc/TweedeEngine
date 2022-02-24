#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Image/TeTexture.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace te
{
    class ImGuiExt
    {
    public:
        enum class ComboOptionFlag
        {
            ShowTexture = 0x1,
            Disable     = 0x2
        };

        template<typename T>
        struct ComboOption
        {
            T Key;
            String Label;
            SPtr<Texture> Tex;

            ComboOption(T key, const String& label, const SPtr<Texture> texture)
                : Key(key)
                , Label(label)
                , Tex(texture)
            { }
        };

        template<typename T>
        struct ComboOptions
        {
            Vector<ComboOption<T>> Options;

            ComboOptions()
            { }

            void AddOption(T key, const String& label, const SPtr<Texture> texture = nullptr)
            {
                Options.push_back(ComboOption<T>(key, label, texture));
            }

            void AddOption(const ComboOption<T>& option)
            {
                Options.push_back(option);
            }

            const ComboOption<T>& operator[](T key)
            {
                for (const auto& option : Options)
                {
                    if (option.Key == key)
                        return option;
                }

                assert(false);
                return Options[0];
            }
        };

    public:
        static bool RenderOptionFloat(float& value, const char* id, const char* text, float min = 0.0f, 
            float max = std::numeric_limits<float>::max() / 2, float width = 0.0f, bool disable = false);

        static bool RenderOptionBool(bool& value, const char* id, const char* text, 
            bool disable = false);

        static bool RenderVector2(Vector2& vector, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderVector3(Vector3& vector, const char* id, const char* text, float width = 0.0f, 
            bool disable = false);

        static bool RenderTransform(Transform& transform, const char* text, 
            bool disable = false);

        static bool RenderColorRGB(Vector4& color, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderColorRGBA(Vector4& color, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderCameraGraphics(HCamera& camera, SPtr<RenderSettings> cameraSettings, 
            float width = 0.0f);

        static bool RenderCameraPostProcessing(HCamera& camera, SPtr<RenderSettings> cameraSettings, 
            float width = 0.0f);

        static bool RenderOptionComboComponent(HCamera* value, const char* id, const char* text, ComboOptions<HCamera>& options,
            float width = 0.0f);

        static void RenderImage(SPtr<Texture> texture, UINT32 maxMip, const Vector2& size, 
            const Vector2& offset = Vector2::ZERO);

        template<typename T>
        static bool RenderOptionInt(T& value, const char* id, const char* text, T min = 0,
            T max = std::numeric_limits<T>::max(), float width = 0.0f, bool disable = false);

        template<typename T>
        static bool RenderOptionCombo(T* value, const char* id, const char* text, ComboOptions<T>& options,
            float width = 0.0f, UINT8 flags = 0)
        {
            if (!Math::ApproxEquals(width, 0.0f) && width < 75.0f)
                width = 75.0f;

            if (flags & (UINT8)ImGuiExt::ComboOptionFlag::Disable)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            bool hasChanged = false;
            ImGui::PushID(id);
            if (width > 0.0f) ImGui::PushItemWidth(width);

            if (ImGui::BeginCombo(text, options[*value].Label.c_str()))
            {
                UINT32 line = 0;
                for (const auto& option : options.Options)
                {
                    ImGui::PushID(line);
                    const bool isSelected = (*value == option.Key);

                    if (flags & (UINT8)ImGuiExt::ComboOptionFlag::ShowTexture && option.Tex &&
                        option.Tex->GetProperties().GetTextureType() == TextureType::TEX_TYPE_2D)
                    {
                        String hiddenLabel = "##" + option.Label;
                        if (ImGui::Selectable(hiddenLabel.c_str(), isSelected, 0, ImVec2(0.0f, 54.0f)))
                        {
                            *value = option.Key;
                            hasChanged = true;
                        }
                        ImGui::SameLine();

                        RenderImage(option.Tex, 4, Vector2(50.0f, 50.0f), Vector2(-5.0f, 2.0f));
                        ImGui::SameLine();

                        ImVec2 cursor = ImGui::GetCursorPos();
                        cursor.y += 21.0f;
                        ImGui::SetCursorPos(cursor);

                        ImGui::Text("%s", option.Label.c_str());

                        cursor = ImGui::GetCursorPos();
                        cursor.y -= 21.0f;
                        ImGui::SetCursorPos(cursor);
                    }
                    else
                    {
                        if (ImGui::Selectable(option.Label.c_str(), isSelected))
                        {
                            *value = option.Key;
                            hasChanged = true;
                        }
                    }

                    ImGui::PopID();

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();

                    line++;
                }

                ImVec2 cursor = ImGui::GetCursorPos();
                cursor = ImGui::GetCursorPos();
                cursor.y = 15.0f;

                ImGui::EndCombo();
            }
            if (width > 0.0f) ImGui::PopItemWidth();
            ImGui::PopID();

            if (flags & (UINT8)ImGuiExt::ComboOptionFlag::Disable)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            return hasChanged;
        };
    };
}
