#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Scene/TeTransform.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Components/TeCCamera.h"

namespace te
{
    class ImGuiExt
    {
    public:
        template<typename T>
        struct ComboOption
        {
            T Key;
            String Label;

            ComboOption(T key, const String& label)
                : Key(key)
                , Label(label)
            { }
        };

        template<typename T>
        struct ComboOptions
        {
            Vector<ComboOption<T>> Options;

            ComboOptions()
            { }

            void AddOption(T key, const String& label)
            {
                Options.push_back(ComboOption<T>(key, label));
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
            float max = std::numeric_limits<float>::max(), float width = 0.0f, bool disable = false);

        static bool RenderOptionInt(int& value, const char* id, const char* text, int min = 0, 
            int max = std::numeric_limits<int>::max(), float width = 0.0f, bool disable = false);

        static bool RenderOptionBool(bool& value, const char* id, const char* text, 
            bool disable = false);

        static bool RenderVector2(Vector2& vector, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderVector3(Vector3& vector, const char* id, const char* text, float width = 0.0f, 
            bool disable = false);

        static bool RenderTransform(Transform& transform, const char* text, 
            bool disable = false);

        static bool RenderColorRGBA(Vector4& color, const char* id, const char* text, float width = 0.0f,
            bool disable = false);

        static bool RenderCameraGraphics(HCamera& camera, SPtr<RenderSettings> cameraSettings, 
            float width = 0.0f);

        static bool RenderCameraPostProcessing(HCamera& camera, SPtr<RenderSettings> cameraSettings, 
            float width = 0.0f);

        static bool RenderOptionComboComponent(HCamera* value, const char* id, const char* text, ComboOptions<HCamera>& options,
            float width = 0.0f);

        template<typename T>
        static bool RenderOptionCombo(T* value, const char* id, const char* text, ComboOptions<T>& options,
            float width = 0.0f, bool disable = false)
        {
            if (width != 0.0f && width < 75.0f)
                width = 75.0f;

            if (disable)
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
                    if (ImGui::Selectable(option.Label.c_str(), isSelected))
                    {
                        *value = option.Key;
                        hasChanged = true;
                    }
                    ImGui::PopID();

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();

                    line++;
                }
                ImGui::EndCombo();
            }
            if (width > 0.0f) ImGui::PushItemWidth(width);
            ImGui::PopID();

            if (disable)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            return hasChanged;
        };
    };
}
