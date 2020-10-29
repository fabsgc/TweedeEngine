#include "TeImGuiExt.h"

#include "Math/TeMath.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Math/TeQuaternion.h"
#include "Math/TeRadian.h"

namespace te
{
    bool ImGuiExt::RenderOptionFloat(float& value, const char* id, const char* text, 
        float min, float max, float width)
    {
        const float previousValue = value;

        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::SliderFloat(text, &value, min, max, "%.2f");
        if (width > 0.0f) ImGui::PopItemWidth();
        ImGui::PopID();
        value = Math::Clamp(value, min, max);

        if (previousValue != value)
            return true;

        return false;
    };

    bool ImGuiExt::RenderOptionInt(int& value, const char* id, const char* text,
        int min, int max, float width)
    {
        const int previousValue = value;

        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::SliderInt(text, &value, min, max, "%.d");
        if (width > 0.0f) ImGui::PopItemWidth();
        ImGui::PopID();
        value = Math::Clamp(value, min, max);

        if (previousValue != value)
            return true;

        return false;
    };

    bool ImGuiExt::RenderOptionBool(bool& value, const char* id, const char* text)
    {
        const bool previousValue = value;

        ImGui::PushID(id);
        ImGui::Checkbox(text, &value);
        ImGui::PopID();

        if (previousValue != value)
            return true;

        return false;
    };

    bool ImGuiExt::RenderOptionCombo(int* value, const char* id, const char* text,
        Vector<int>& options, Vector<String>& labels, float width)
    {
        if (width != 0.0f && width < 75.0f) 
            width = 75.0f;

        bool hasChanged = false;
        ImGui::PushID(id);
        if (width > 0.0f) ImGui::PushItemWidth(width);
        if (ImGui::BeginCombo(text, labels[*value].c_str()))
        {
            for (size_t i = 0; i < options.size(); i++)
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
        if (width > 0.0f) ImGui::PushItemWidth(width);
        ImGui::PopID();

        return hasChanged;
    };

    bool ImGuiExt::RenderVector3(Vector3& vector, const char* id, const char* text, float width)
    {
        bool hasChanged = false;
        auto& io = ImGui::GetIO();
        auto& ctx = *ImGui::GetCurrentContext();

        float buttonsWidth = ImGui::CalcItemWidth() - width;
        float lineHeight = ctx.Font->FontSize + ctx.Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 20.0f, lineHeight };

        buttonsWidth = (buttonsWidth < 120.0f) ? 120.0f : buttonsWidth;

        if (fabs(vector.x) == 0.0f) vector.x = 0.0f;
        if (fabs(vector.y) == 0.0f) vector.y = 0.0f;
        if (fabs(vector.z) == 0.0f) vector.z = 0.0f;

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

        return hasChanged;
    };

    bool ImGuiExt::RenderTransform(Transform& transform, const char* text)
    {
        bool hasChanged = false;

        Radian x, y, z;
        transform.GetRotation().ToEulerAngles(x, y, z);

        Vector3 position = transform.GetPosition();
        Vector3 rotation(x.ValueDegrees(), y.ValueDegrees(), z.ValueDegrees());
        Vector3 scale = transform.GetScale();

        ImGui::PushID(text);
            if (RenderVector3(position, "##transform_position_option", " Position", 20.0f))
            {
                transform.SetPosition(position);
                hasChanged = true;
            }
            if (RenderVector3(rotation, "##transform_rotation_option", " Rotation", 20.0f))
            {
                Quaternion rot;
                rot.FromEulerAngles(Radian(Degree(rotation.x)), Radian(Degree(rotation.y)), Radian(Degree(rotation.z)));
                transform.SetRotation(rot);
                hasChanged = true;
            }
            if (RenderVector3(scale, "##transform_scale_option", " Scale", 20.0f))
            {
                transform.SetScale(scale);
                hasChanged = true;
            }
        ImGui::PopID();

        return hasChanged;
    };
}
