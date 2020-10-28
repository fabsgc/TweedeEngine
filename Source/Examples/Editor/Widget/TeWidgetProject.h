#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "../TeEditor.h"
#include "Renderer/TeLight.h"
#include "Input/TeVirtualInput.h"

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
        enum class RenderableType
        {
            Empty, Cube, Sphere, Cylinder, Cone
        };

    protected:
        void ShowTree(const HSceneObject& sceneObject);
        void ShowSceneObjectTree(const HSceneObject& sceneObject, bool expand = false);
        void ShowComponentsTree(const HSceneObject& sceneObject);
        void OnTreeBegin();
        void OnTreeEnd();
        void HandleClicking();
        void HandleKeyShortcuts();
        void HandleSelectionWindowSwitch();
        void SetSelectedSceneObject(SPtr<SceneObject> sceneObject);
        void SetSelectedComponent(SPtr<Component> component);
        void Popups();
        void PopupContextMenu();
        void CreateSceneObject();
        void CreateRenderable(RenderableType type);
        void CreateLight(LightType type);
        void CreateCamera(TypeID_Core type);
        void CreateAudio();
        void CreateScript();
        void CreateSkybox();
        void Paste();
        void Delete();
        String GetComponentIcon(const HComponent& component);

    protected:
        Editor::SelectionData& _selections;
        bool _expandToSelection;
        bool _expandedToSelection;
        ImRect _selectedSceneObjectRect;

        VirtualButton _deleteBtn;
        VirtualButton _copyBtn;
        VirtualButton _pasteBtn;
    };
}
