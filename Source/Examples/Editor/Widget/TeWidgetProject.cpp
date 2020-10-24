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

        auto children = sceneSO->GetChildren();
        auto components = sceneSO->GetComponents();

        for (auto& component : components)
        {

        }

        for (auto& child : children)
        {

        }
    }

    void WidgetProject::UpdateBackground()
    { }
}
 