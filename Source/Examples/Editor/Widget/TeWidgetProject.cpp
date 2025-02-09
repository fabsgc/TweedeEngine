#include "TeWidgetProject.h"

#include "../TeEditorUtils.h"
#include "Project/TeProject.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-security"  // warning: format string is not a string literal (potentially insecure)
#endif

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
    { 
        _title = PROJECT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    { }

    void WidgetProject::Update()
    { 
        char inputName[256];
        char inputPath[512];
        char inputUUID[64];
        bool hasChanged = false;
        String name = gEditor().GetProject()->GetName();
        String path = gEditor().GetProject()->GetPath();
        String uuid = gEditor().GetProject()->GetUUID().ToString();
        const float widgetWidth = ImGui::GetWindowContentRegionWidth() - 100.0f;

        if (name.length() < 256) strcpy(inputName, name.c_str());
        else strcpy(inputName, name.substr(0, 255).c_str());

        if (path.length() < 256) strcpy(inputPath, path.c_str());
        else strcpy(inputPath, path.substr(0, 255).c_str());

        strcpy(inputUUID, uuid.c_str());

        if (ImGui::CollapsingHeader("Identification", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(widgetWidth);
            if (ImGui::InputText("Name", inputName, IM_ARRAYSIZE(inputName)))
            {
                gEditor().GetProject()->SetName(inputName);
            }

            if (ImGui::InputText("Path", inputPath, IM_ARRAYSIZE(inputPath)))
            {
                gEditor().GetProject()->SetPath(path);
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", path.c_str());

            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::InputText("UUID", inputUUID, IM_ARRAYSIZE(inputUUID));
            ImGui::PopItemFlag();
            ImGui::PopItemWidth();
        }
    }

    void WidgetProject::UpdateBackground()
    { }
}
 