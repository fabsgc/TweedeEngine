#include "TeWidgetProject.h"

#include "Scene/TeSceneObject.h"

#include "../TeEditor.h"

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
    { 
        _title = PROJECT_TITLE;
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    { }

    void WidgetProject::Update()
    { 
        HSceneObject& sceneSO = Editor::Instance().GetSceneRoot();
        ShowTree(sceneSO, true);
    }

    void WidgetProject::UpdateBackground()
    { }

    void WidgetProject::ShowTree(const HSceneObject& sceneObject, bool expand)
    {
        UINT64 sceneObjectId = sceneObject->GetInstanceId();

        auto children = sceneObject->GetChildren();
        auto components = sceneObject->GetComponents();

        // Flags
        ImGuiTreeNodeFlags nodeFlags = 
            ImGuiTreeNodeFlags_AllowItemOverlap | 
            ImGuiTreeNodeFlags_SpanAvailWidth | 
            ImGuiTreeNodeFlags_Framed;

        // Should we expand this node ?
        if (expand) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        // Flag - Is expandable (has children) ?
        nodeFlags |= (children.size() || components.size()) ? 
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;

        // Title
        const char * nodeIcon = ICON_FA_GLOBE_EUROPE " ";
        String nodeTitle = nodeIcon + sceneObject->GetName();

        const bool isNodeOpened = ImGui::TreeNodeEx(
            reinterpret_cast<void*>(static_cast<intptr_t>(sceneObjectId)), nodeFlags, nodeTitle.c_str());

        // Recursively show all child nodes
        if (isNodeOpened)
        {
            if (components.size() > 0)
                ShowComponentTree(sceneObject);

            for (auto& child : children)
            {
                ShowTree(child);
            }

            // Pop if isNodeOpen
            ImGui::TreePop();
        }
    }

    void WidgetProject::ShowComponentTree(const HSceneObject& sceneObject)
    {
        auto components = sceneObject->GetComponents();

        for (auto& component : components)
        {
            UINT64 componentId = component->GetInstanceId();
            ImGuiTreeNodeFlags componentFlags =
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_FramePadding;

            const char * componentIcon = ICON_FA_SHAPES " ";
            String componentTitle = componentIcon + sceneObject->GetName();

            if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(componentId)), componentFlags, componentTitle.c_str()))
            {
                ImGui::TreePop();
            }
        }
    }
}
 