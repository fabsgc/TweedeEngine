#include "TeWidgetProject.h"
#include "../TeEditor.h"

#include "Resources/TeResourceManager.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"

namespace te
{
    const String WidgetProject::DELETE_BINDING = "Delete";
    const String WidgetProject::COPY_BINDING = "Copy";
    const String WidgetProject::PASTE_BINDING = "Paste";

    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
        , _selections(gEditor().GetSelectionData())
        , _expandToSelection(false)
        , _expandedToSelection(false)
    { 
        _title = PROJECT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    { 
        _deleteBtn = VirtualButton(DELETE_BINDING);
        _copyBtn = VirtualButton(COPY_BINDING);
        _pasteBtn = VirtualButton(PASTE_BINDING);
    }

    void WidgetProject::Update()
    { 
        HSceneObject& sceneSO = gEditor().GetSceneRoot();
        ShowTree(sceneSO);
    }

    void WidgetProject::UpdateBackground()
    { }

    void WidgetProject::ShowTree(const HSceneObject& sceneObject)
    {
        OnTreeBegin();
        ShowSceneObjectTree(sceneObject, true);

        // If we have been expanding to show an entity and no more expansions are taking place, we reached it.
        // So, we stop expanding and we bring it into view.
        if (_expandToSelection && !_expandedToSelection)
        {
            ImGui::ScrollToBringRectIntoView(_window, _selectedSceneObjectRect);
            _expandToSelection = false;
        }

        OnTreeEnd();
    }

    void WidgetProject::ShowSceneObjectTree(const HSceneObject& sceneObject, bool expand)
    {
        UINT64 sceneObjectId = sceneObject->GetInstanceId();
        auto children = sceneObject->GetChildren();
        auto components = sceneObject->GetComponents();
        _expandedToSelection = false;

        // Flags
        ImGuiTreeNodeFlags nodeFlags =
            ImGuiTreeNodeFlags_AllowItemOverlap |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_Framed;

        // Should we expand this node ?
        if (expand) 
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        // Flag - Is expandable (has children) ?
        nodeFlags |= (children.size() || components.size()) ?
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;

        // Flag - Is selected?
        if (_selections.ClickedSceneObject)
        {
            nodeFlags |= (_selections.ClickedSceneObject->GetUUID() == sceneObject->GetUUID()) ? ImGuiTreeNodeFlags_Selected : nodeFlags;

            if (_expandToSelection)
            {
                // If the selected sceneObject is a descendant of the current one, start expanding (this can happen if an object is selected in the viewport)
                if (_selections.ClickedSceneObject->IsDescendantOf(sceneObject))
                {
                    ImGui::SetNextItemOpen(true);
                    _expandedToSelection = true;
                }
            }
        }

        if (_selections.ClickedComponent && _expandToSelection)
        {
            // If the selected sceneObject is a descendant of the current one, start expanding (this can happen if an object is selected in the viewport)
            if (_selections.ClickedComponent->IsDescendantOf(sceneObject))
            {
                ImGui::SetNextItemOpen(true);
                _expandedToSelection = true;
            }
        }

        // Title
        String active = (sceneObject->GetActive()) ? ICON_FA_EYE : ICON_FA_EYE_SLASH;
        String nodeTitle = active + String("  ") + ICON_FA_GLOBE_EUROPE + String(" ") + sceneObject->GetName();

        if (_selections.ClickedSceneObject == sceneObject.GetInternalPtr())
            nodeTitle += String("  ") + ICON_FA_CARET_RIGHT;

        if (components.size() == 0 && children.size() == 0)
            nodeTitle = "          " + nodeTitle;

        const bool isNodeOpened = ImGui::TreeNodeEx(
            reinterpret_cast<void*>(static_cast<intptr_t>(sceneObjectId)), nodeFlags, nodeTitle.c_str());

        // Keep a copy of the selected item's rect so that we can scroll to bring it into view
        if ((nodeFlags & ImGuiTreeNodeFlags_Selected) && _expandToSelection)
            _selectedSceneObjectRect = _window->DC.LastItemRect;

        // Manually detect some useful states
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
            _selections.HoveredSceneObject = sceneObject.GetInternalPtr();

        // Recursively show all child nodes
        if (isNodeOpened)
        {
            if (components.size() > 0)
                ShowComponentsTree(sceneObject);

            for (auto& child : children)
                ShowSceneObjectTree(child);

            // Pop if isNodeOpen
            ImGui::TreePop();
        }
    }

    void WidgetProject::ShowComponentsTree(const HSceneObject& sceneObject)
    {
        auto components = sceneObject->GetComponents();
        _expandedToSelection = false;

        for (auto& component : components)
        {
            UINT64 componentId = component->GetInstanceId();
            ImGuiTreeNodeFlags componentFlags =
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_FramePadding;

            // Flag - Is selected?
            if (_selections.ClickedComponent)
            {
                componentFlags |= (_selections.ClickedComponent->GetUUID() == component->GetUUID()) ? ImGuiTreeNodeFlags_Selected : componentFlags;

                if (_expandToSelection)
                {
                    // If the selected component is a descendant of the current one, but not its direct child
                    // start expanding (this can happen if an object is selected in the viewport)
                    if (!_selections.ClickedComponent->IsChildOf(sceneObject) && _selections.ClickedComponent->IsDescendantOf(sceneObject))
                    {
                        ImGui::SetNextItemOpen(true);
                        _expandedToSelection = true;
                    }
                }
            }

            if (_selections.ClickedComponent == component.GetInternalPtr())
                componentFlags |= ImGuiTreeNodeFlags_Selected;

            String componentIcon = GetComponentIcon(component);

            if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(componentId)), componentFlags, componentIcon.c_str()))
            {
                // Manually detect some useful states
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
                    _selections.HoveredComponent = component.GetInternalPtr();

                // Keep a copy of the selected item's rect so that we can scroll to bring it into view
                if ((componentFlags & ImGuiTreeNodeFlags_Selected) && _expandToSelection)
                    _selectedSceneObjectRect = _window->DC.LastItemRect;

                ImGui::TreePop();
            }
        }
    }

    void WidgetProject::OnTreeBegin()
    {
        _selections.HoveredSceneObject = nullptr;
        _selections.HoveredComponent = nullptr;
    }

    void WidgetProject::OnTreeEnd()
    {
        HandleClicking();
        HandleKeyShortcuts();
        Popups();
    }

    void WidgetProject::HandleClicking()
    {
        const auto isWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        const auto leftClick = ImGui::IsMouseClicked(0);
        const auto rightClick = ImGui::IsMouseClicked(1);

        // Since we are handling clicking manually, we must ensure we are inside the window
        if (!isWindowHovered)
            return;

        // Left click on SceneObject
        if (leftClick && _selections.HoveredSceneObject)
            SetSelectedSceneObject(_selections.HoveredSceneObject);

        // Left click on Component
        if (leftClick && _selections.HoveredComponent)
            SetSelectedComponent(_selections.HoveredComponent);

        // Right click on item - Select and show context menu
        if (ImGui::IsMouseClicked(1))
        {
            if (_selections.HoveredSceneObject)
                SetSelectedSceneObject(_selections.HoveredSceneObject);

            if (_selections.HoveredComponent)
                SetSelectedComponent(_selections.HoveredComponent);

            ImGui::OpenPopup("##ProjectContextMenu");
        }

        // Clicking on empty space - Clear selection
        if ((leftClick || rightClick) && !_selections.HoveredComponent && !_selections.HoveredSceneObject)
        {
            _selections.ClickedSceneObject = nullptr;
            _selections.ClickedComponent = nullptr;
        }
    }

    void WidgetProject::HandleKeyShortcuts()
    {
        if (!ImGui::IsWindowHovered())
            return;

        if (gVirtualInput().IsButtonDown(_copyBtn))
        {
            if (_selections.ClickedComponent)
                _selections.CopiedComponent = _selections.ClickedComponent;
            else
                _selections.CopiedSceneObject = _selections.ClickedSceneObject;
        }

        if (gVirtualInput().IsButtonDown(_pasteBtn))
            Paste();

        if (gVirtualInput().IsButtonDown(_deleteBtn))
            Delete();
    }

    void WidgetProject::HandleSelectionWindowSwitch()
    {
        if (_selections.ClickedComponent)
        {
            switch (_selections.ClickedComponent->GetCoreType())
            {
            case TID_CCamera:
            case TID_CCameraUI:
            case TID_CCameraFlyer:
            case TID_CRenderable:
            case TID_CLight:
            case TID_CSkybox:
                gEditor().PutFocus(Editor::WindowType::Viewport);
                break;

            case TID_CScript:
                gEditor().PutFocus(Editor::WindowType::Script);
                break;
            }
        }
    }

    void WidgetProject::SetSelectedSceneObject(SPtr<SceneObject> sceneObject)
    {
        _selections.ClickedSceneObject = sceneObject;
        _selections.ClickedComponent = nullptr;
        _expandToSelection = true;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::SetSelectedComponent(SPtr<Component> component)
    {
        _selections.ClickedComponent = component;
        _selections.ClickedSceneObject = component->GetSceneObject().GetInternalPtr();
        _expandToSelection = true;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::Popups()
    {
        PopupContextMenu();
    }

    void WidgetProject::PopupContextMenu()
    {
        if (!ImGui::BeginPopup("##ProjectContextMenu"))
            return;

        if (!_selections.ClickedComponent)
        {
            if (ImGui::MenuItem(ICON_FA_GLOBE_EUROPE " Add SceneObject"))
                CreateSceneObject();

            if (_selections.ClickedSceneObject)
            {
                if (ImGui::BeginMenu(ICON_FA_PLUS_SQUARE " Add Component"))
                {
                    if (ImGui::MenuItem(ICON_FA_OBJECT_GROUP " Renderable"))
                        CreateRenderable(RenderableType::Empty);

                    if (ImGui::BeginMenu(ICON_FA_LIGHTBULB " Light"))
                    {
                        if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Directional light"))
                            CreateLight(LightType::Directional);

                        if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Point light"))
                            CreateLight(LightType::Radial);

                        if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Spot light"))
                            CreateLight(LightType::Spot);

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu(ICON_FA_CAMERA " Camera"))
                    {
                        if (ImGui::MenuItem(ICON_FA_CAMERA " Rendering camera"))
                            CreateCamera(TypeID_Core::TID_CCamera);

                        if (ImGui::MenuItem(ICON_FA_CAMERA " Flying camera"))
                            CreateCamera(TypeID_Core::TID_CCameraFlyer);

                        if (ImGui::MenuItem(ICON_FA_CAMERA" Orbital camera"))
                            CreateCamera(TypeID_Core::TID_CCameraUI);

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu(ICON_FA_VOLUME_UP " Audio"))
                    {
                        if (ImGui::MenuItem(ICON_FA_MICROPHONE " Audio source"))
                            CreateAudio();

                        if (ImGui::MenuItem(ICON_FA_HEADPHONES " Audio listener"))
                            CreateAudio();

                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem(ICON_FA_GLOBE " Skybox"))
                        CreateSkybox();

                    if (ImGui::MenuItem(ICON_FA_SCROLL " Script"))
                        CreateScript();

                    ImGui::EndMenu();
                }
            }
        }

        if ((_selections.ClickedSceneObject && _selections.ClickedSceneObject != gEditor().GetSceneRoot().GetInternalPtr())
            || _selections.ClickedComponent)
        {
            if (ImGui::MenuItem(ICON_FA_COPY " Copy"))
            {
                if (_selections.ClickedComponent)
                    _selections.CopiedComponent = _selections.ClickedComponent;
                else
                    _selections.CopiedSceneObject = _selections.ClickedSceneObject;
            }
        }

        if (_selections.CopiedSceneObject || _selections.CopiedComponent)
        {
            if (ImGui::MenuItem(ICON_FA_PASTE " Paste"))
                Paste();
        }

        if ((_selections.ClickedSceneObject && _selections.ClickedSceneObject != gEditor().GetSceneRoot().GetInternalPtr())
            || _selections.ClickedComponent)
        {
            if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
                Delete();
        }

        ImGui::EndPopup();
    }

    void WidgetProject::CreateSceneObject()
    { 
        HSceneObject sceneObject = SceneObject::Create("SceneObject");

        if (_selections.ClickedSceneObject)
            sceneObject->SetParent(_selections.ClickedSceneObject->GetHandle());
        else
            sceneObject->SetParent(gEditor().GetSceneRoot());

        _expandToSelection = true;
        _selections.ClickedSceneObject = sceneObject.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateRenderable(RenderableType type)
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HRenderable renderable = _selections.ClickedSceneObject->AddComponent<CRenderable>();

        switch (type)
        {
        case RenderableType::Empty:
            renderable.Get()->SetName("Empty renderable");
            renderable.Get()->Initialize();
            break;

        default:
            break;
        }

        _expandToSelection = true;
        _selections.ClickedComponent = renderable.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateLight(LightType type)
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HLight light = _selections.ClickedSceneObject->AddComponent<CLight>(type);

        switch (type)
        {
        case LightType::Directional:
            light.Get()->SetName("Directional");
            light.Get()->Initialize();
            break;

        case LightType::Radial:
            light.Get()->SetName("Point");
            light.Get()->Initialize();
            break;

        case LightType::Spot:
            light.Get()->SetName("Spot");
            light.Get()->Initialize();
            break;

        default:
            break;
        }

        _expandToSelection = true;
        _selections.ClickedComponent = light.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateCamera(TypeID_Core type)
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        switch (type)
        {
        case TID_CCamera:
            {
                HCamera camera = _selections.ClickedSceneObject->AddComponent<CCamera>();
                camera->SetName("Camera");
                camera->Initialize();
                _selections.ClickedComponent = camera.GetInternalPtr();
            }
            break;

        case TID_CCameraFlyer:
            {
                HCameraFlyer camera = _selections.ClickedSceneObject->AddComponent<CCameraFlyer>();
                camera->SetName("Flying Camera");
                camera->Initialize();
                _selections.ClickedComponent = camera.GetInternalPtr();
            }
            break;

        case TID_CCameraUI:
            {
                HCameraUI camera = _selections.ClickedSceneObject->AddComponent<CCameraUI>();
                camera->SetName("Orbital Camera");
                camera->Initialize();
                _selections.ClickedComponent = camera.GetInternalPtr();
            }
            break;

        default:
            break;
        }

        _expandToSelection = true;
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateAudio()
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        // TODO

        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateScript()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HScript script = _selections.ClickedSceneObject->AddComponent<CScript>();
        script.Get()->SetName("Script");
        script.Get()->Initialize();

        _expandToSelection = true;
        _selections.ClickedComponent = script.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::CreateSkybox()
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (SceneManager::Instance().FindComponents<CSkybox>().size() > 0)
            return;

        HSkybox skybox = _selections.ClickedSceneObject->AddComponent<CSkybox>();
        skybox.Get()->SetName("Skybox");
        skybox.Get()->Initialize();

        _expandToSelection = true;
        _selections.ClickedComponent = skybox.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::Paste()
    { 
        if (!_selections.CopiedSceneObject && !_selections.CopiedComponent)
            return;

        SPtr<SceneObject> clickedSceneObject = _selections.ClickedSceneObject ? 
            _selections.ClickedSceneObject : gEditor().GetSceneRoot().GetInternalPtr();

        if (_selections.CopiedComponent)
        {
            switch (_selections.CopiedComponent->GetCoreType())
            {
            case TID_CCamera:
                {
                    HCamera component = clickedSceneObject->AddComponent<CCamera>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            case TID_CCameraFlyer:
                {
                    HCameraFlyer component = clickedSceneObject->AddComponent<CCameraFlyer>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            case TID_CCameraUI:
                {
                    HCameraUI component = clickedSceneObject->AddComponent<CCameraUI>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            case TID_CLight:
                {
                    HLight component = clickedSceneObject->AddComponent<CLight>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            case TID_CRenderable:
                {
                    HRenderable component = clickedSceneObject->AddComponent<CRenderable>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            case TID_CScript:
            {
                HScript component = clickedSceneObject->AddComponent<CScript>();
                component->Clone(_selections.CopiedComponent->GetHandle());
                component->Initialize();
                _selections.ClickedComponent = component.GetInternalPtr();
                _selections.CopiedComponent = component.GetInternalPtr();
            }
            break;

            case TID_CSkybox:
                {
                    if (SceneManager::Instance().FindComponents<CSkybox>().size() > 0)
                        break;

                    HSkybox component = clickedSceneObject->AddComponent<CSkybox>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

            default:
                break;
            }
        }
        else if (_selections.CopiedSceneObject)
        {
            if (_selections.CopiedSceneObject == gEditor().GetSceneRoot().GetInternalPtr())
                return;

            HSceneObject sceneObject = SceneObject::Create("SceneObject");
            sceneObject->Clone(_selections.CopiedSceneObject);
            sceneObject->SetParent(clickedSceneObject->GetHandle());

            _selections.ClickedSceneObject = sceneObject.GetInternalPtr();
            _selections.CopiedSceneObject = sceneObject.GetInternalPtr();
        }

        _expandToSelection = true;
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
        HandleSelectionWindowSwitch();
    }

    void WidgetProject::Delete()
    { 
        if (!_selections.ClickedSceneObject && !_selections.ClickedComponent)
            return;

        if (_selections.ClickedComponent)
        {
            if (_selections.CopiedComponent == _selections.ClickedComponent)
                _selections.CopiedComponent = nullptr;

            if (_selections.HoveredComponent == _selections.ClickedComponent)
                _selections.HoveredComponent = nullptr;

            _selections.ClickedComponent->GetSceneObject()->DestroyComponent(_selections.ClickedComponent.get(), true);
            _selections.ClickedComponent = nullptr;
        }
        else if (_selections.ClickedSceneObject)
        {
            if (_selections.ClickedSceneObject == gEditor().GetSceneRoot().GetInternalPtr())
                return;

            _selections.CopiedComponent = nullptr;
            _selections.HoveredComponent = nullptr;

            if (_selections.CopiedSceneObject == _selections.ClickedSceneObject)
                _selections.CopiedSceneObject = nullptr;

            if (_selections.HoveredSceneObject == _selections.ClickedSceneObject)
                _selections.HoveredSceneObject = nullptr;

            _selections.ClickedSceneObject->Destroy(true);
            _selections.ClickedSceneObject = nullptr;
        }

        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    String WidgetProject::GetComponentIcon(const HComponent& component)
    {
        String title = (component->SO()->GetActive()) ? ICON_FA_EYE : ICON_FA_EYE_SLASH;
        UINT32 type = component->GetCoreType();

        switch (type)
        {
        case TID_Component:
            title +=  String("  ") + ICON_FA_SHAPES;
            break;
        case TID_CCamera:
            title += String("  ") + ICON_FA_CAMERA;
            break;
        case TID_CRenderable:
            title += String("  ") + ICON_FA_OBJECT_GROUP;
            break;
        case TID_CLight:
            title += String("  ") + ICON_FA_LIGHTBULB;
            break;
        case TID_CSkybox:
            title += String("  ") + ICON_FA_GLOBE ;
            break;
        case TID_CCameraFlyer:
            title += String("  ") + ICON_FA_CAMERA;
            break;
        case TID_CCameraUI:
            title += String("  ") + ICON_FA_CAMERA;
            break;
        case TID_CScript:
            title += String("  ") + ICON_FA_SCROLL;
            break;
        default:
            break;
        }

        title = title + String("  ") + component->GetName();
        return title;
    }
}
 