#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "../TeEditor.h"

namespace te
{
    class WidgetProject : public Widget
    {
    public:
        WidgetProject();
        ~WidgetProject();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        void ShowTree(const HSceneObject& sceneObject);
        void ShowSceneObjectTree(const HSceneObject& sceneObject, bool expand = false);
        void ShowComponentsTree(const HSceneObject& sceneObject);
        void OnTreeBegin();
        void OnTreeEnd();
        void HandleClicking();
        void SetSelectedSceneObject(SPtr<SceneObject> sceneObject);
        void SetSelectedComponent(SPtr<Component> component);
        void Popups();
        void PopupContextMenu() const;

        String GetComponentIcon(const HComponent& component);

    protected:
        Editor::SelectionData& _selections;
    };
}
