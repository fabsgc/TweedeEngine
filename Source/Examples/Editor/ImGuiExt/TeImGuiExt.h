#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#   define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "TeCorePrerequisites.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Image/TeTexture.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include <functional>

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
        static ImVec2 GetButtonSize(const String& button_text);

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

        static void RenderImage(SPtr<Texture> texture, const Vector2& size, 
            const Vector2& offset = Vector2::ZERO);

        static void RenderButton(const char* titleActive, const char* titleInactive, const std::function<bool()>& isActive, const std::function<bool()>& isEnabled,
            const std::function<void()>& action, const ImVec2& size = ImVec2(), const String& tooltip = String());

        template<typename PredYes, typename PredNo, typename PredCustom>
        static bool RenderYesNo(const char* id, PredYes predYes, PredNo predNo, PredCustom predCustom, const String& message)
        {
            ImVec2 window_size(500, 0);
            bool ret_val = false;

            ImGui::SetNextWindowSize(window_size);
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal(id, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
            {
                ImGui::TextWrapped("%s", message.c_str());
                ImGui::Separator();

                predCustom();

                float buttons_width = GetButtonSize("Yes").x + GetButtonSize("No").x + ImGui::GetStyle().ItemSpacing.x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2.0f - buttons_width / 2.0f - ImGui::GetStyle().WindowPadding.x);

                if (ImGui::Button("Yes", GetButtonSize("Yes")))
                {
                    predYes();
                    ImGui::CloseCurrentPopup();
                    ret_val = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("No", GetButtonSize("No")))
                {
                    predNo();
                    ImGui::CloseCurrentPopup();
                    ret_val = false;
                }
                ImGui::EndPopup();
            }

            return ret_val;
        }

        template<typename PredOk, typename PredCustom>
        static bool RenderMessage(const char* id, PredOk predOk, PredCustom predCustom, const String& message)
        {
            ImVec2 window_size(500, 0);
            bool ret_val = false;

            ImGui::SetNextWindowSize(window_size);
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal(id, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
            {
                ImGui::TextWrapped("%s", message.c_str());
                ImGui::Separator();

                predCustom();

                ImVec2 button_size = GetButtonSize("OK");
                ImGui::SetCursorPosX(window_size.x / 2.0f - button_size.x / 2.0f);
                if (ImGui::Button("OK", button_size))
                {
                    predOk();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            return ret_val;
        }

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

                        RenderImage(option.Tex, Vector2(50.0f, 50.0f), Vector2(-5.0f, 2.0f));
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
