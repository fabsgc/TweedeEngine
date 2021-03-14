#pragma once

#include "TeCorePrerequisites.h"
#include "../TeEditor.h"
#include "TeWidget.h"
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

        void ForceExpandToSelection() { _expandToSelection = true; }

    protected:
        enum class RenderableType
        {
            Empty
        };

        enum class DragPayloadType
        {
            Component, SceneObject
        };

        struct DragDropPayload
        {
            DragDropPayload()
                : Type(DragPayloadType::SceneObject)
                , Uuid(UUID())
            { }

            DragDropPayload(DragPayloadType type, UUID uuid)
                : Type(type)
                , Uuid(uuid)
            { }

            DragPayloadType Type;
            UUID Uuid;
        };
    
    protected:
        void CreateDragPayload(const DragDropPayload& payload);
        DragDropPayload* ReceiveDragPayload(DragPayloadType type);
        void HandleDragAndDrop(HSceneObject& sceneObject);
        void HandleDragAndDrop(HComponent& component);

    protected:
        void ShowTree(HSceneObject& sceneObject);
        void ShowSceneObjectTree(HSceneObject& sceneObject, bool expand = false);
        void ShowComponentsTree(HSceneObject& sceneObject);
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
        void CreateAudioSource();
        void CreateAudioListener();
        void CreateAnimation();
        void CreateBone();
        void CreateScript();
        void CreateSkybox();
        void Paste();
        void Delete();
        String GetComponentIcon(const HComponent& component);

    protected:
        Editor::SelectionData& _selections;
        bool _expandToSelection;
        bool _expandDragToSelection;
        bool _expandedToSelection;
        bool _handleSelectionWindowSwitch;
        ImRect _selectedSceneObjectRect;

        VirtualButton _deleteBtn;
        VirtualButton _copyBtn;
        VirtualButton _pasteBtn;

        DragDropPayload _dragPayload;
    };
}
