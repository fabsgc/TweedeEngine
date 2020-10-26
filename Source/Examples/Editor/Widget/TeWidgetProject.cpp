#include "TeWidgetProject.h"
#include "Scene/TeSceneObject.h"
#include "../TeEditor.h"

#include "Resources/TeResourceManager.h"
#include "Scene/TeSceneManager.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
        , _selections(Editor::Instance().GetSelectionData())
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
        ShowTree(sceneSO);
    }

    void WidgetProject::UpdateBackground()
    { }

    void WidgetProject::ShowTree(const HSceneObject& sceneObject)
    {
        OnTreeBegin();
        ShowSceneObjectTree(sceneObject, true);
        OnTreeEnd();
    }

    void WidgetProject::ShowSceneObjectTree(const HSceneObject& sceneObject, bool expand)
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
        if (expand) 
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        // Selected style
        if (_selections.ClickedSceneObject == sceneObject.GetInternalPtr())
            nodeFlags |= ImGuiTreeNodeFlags_Selected;

        // Flag - Is expandable (has children) ?
        nodeFlags |= (children.size() || components.size()) ?
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;

        // Title
        String nodeTitle = ICON_FA_GLOBE_EUROPE + String(" ") + sceneObject->GetName();

        if (_selections.ClickedSceneObject == sceneObject.GetInternalPtr())
            nodeTitle += String("  ") + ICON_FA_CARET_RIGHT;

        const bool isNodeOpened = ImGui::TreeNodeEx(
            reinterpret_cast<void*>(static_cast<intptr_t>(sceneObjectId)), nodeFlags, nodeTitle.c_str());

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

        for (auto& component : components)
        {
            UINT64 componentId = component->GetInstanceId();
            ImGuiTreeNodeFlags componentFlags =
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_FramePadding;

            if (_selections.ClickedComponent == component.GetInternalPtr())
                componentFlags |= ImGuiTreeNodeFlags_Selected;

            String componentIcon = GetComponentIcon(component);

            if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(componentId)), componentFlags, componentIcon.c_str()))
            {
                // Manually detect some useful states
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
                    _selections.HoveredComponent = component.GetInternalPtr();

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

    void WidgetProject::SetSelectedSceneObject(SPtr<SceneObject> sceneObject)
    {
        _selections.ClickedSceneObject = sceneObject;
        _selections.ClickedComponent = nullptr;
    }

    void WidgetProject::SetSelectedComponent(SPtr<Component> component)
    {
        _selections.ClickedComponent = component;
        _selections.ClickedSceneObject = component->GetSceneObject().GetInternalPtr();
    }

    void WidgetProject::Popups()
    {
        PopupContextMenu();
    }

    void WidgetProject::PopupContextMenu()
    {
        if (!ImGui::BeginPopup("##ProjectContextMenu"))
            return;

        // EMPTY
        if (!_selections.ClickedComponent)
        {
            if (ImGui::MenuItem(ICON_FA_GLOBE_EUROPE " Add SceneObject"))
                CreateSceneObject();

            if (_selections.ClickedSceneObject)
            {
                if (ImGui::BeginMenu(ICON_FA_PLUS_SQUARE " Add Component"))
                {
                    if (ImGui::BeginMenu(ICON_FA_OBJECT_GROUP " Renderable"))
                    {
                        if (ImGui::MenuItem(ICON_FA_STICKY_NOTE " Empty"))
                            CreateRenderable(RenderableType::Empty);

                        if (ImGui::MenuItem(ICON_FA_CUBE " Cube"))
                            CreateRenderable(RenderableType::Cube);

                        if (ImGui::MenuItem(ICON_FA_CIRCLE " Sphere"))
                            CreateRenderable(RenderableType::Sphere);

                        if (ImGui::MenuItem(ICON_FA_CALENDAR " Cylinder"))
                            CreateRenderable(RenderableType::Cylinder);

                        if (ImGui::MenuItem(ICON_FA_PLAY " Cone"))
                            CreateRenderable(RenderableType::Cone);

                        ImGui::EndMenu();
                    }

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

                    ImGui::EndMenu();
                }
            }
        }

        if ((_selections.ClickedSceneObject && _selections.ClickedSceneObject != Editor::Instance().GetSceneRoot().GetInternalPtr())
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

        if ((_selections.ClickedSceneObject && _selections.ClickedSceneObject != Editor::Instance().GetSceneRoot().GetInternalPtr())
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
            sceneObject->SetParent(Editor::Instance().GetSceneRoot());

        Editor::Instance().NeedsRedraw();
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

        case RenderableType::Cube:
            renderable.Get()->SetName("Cube");
            renderable.Get()->Initialize();
            break;

        case RenderableType::Cylinder:
            renderable.Get()->SetName("Cylinder");
            renderable.Get()->Initialize();
            break;

        case RenderableType::Sphere:
            renderable.Get()->SetName("Sphere");
            renderable.Get()->Initialize();
            break;

        case RenderableType::Cone:
            renderable.Get()->SetName("Cone");
            renderable.Get()->Initialize();
            break;

        default:
            break;
        }

        Editor::Instance().NeedsRedraw();
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

        Editor::Instance().NeedsRedraw();
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
                camera.Get()->SetName("Camera");
                camera.Get()->Initialize();
            }
            break;

        case TID_CCameraFlyer:
            {
                HCameraFlyer camera = _selections.ClickedSceneObject->AddComponent<CCameraFlyer>();
                camera.Get()->SetName("Flying Camera");
                camera.Get()->Initialize();
            }
            break;

        case TID_CCameraUI:
            {
                HCameraUI camera = _selections.ClickedSceneObject->AddComponent<CCameraUI>();
                camera.Get()->SetName("Orbital Camera");
                camera.Get()->Initialize();
            }
            break;

        default:
            break;
        }

        Editor::Instance().NeedsRedraw();
    }

    void WidgetProject::CreateAudio()
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        // TODO

        Editor::Instance().NeedsRedraw();
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

        Editor::Instance().NeedsRedraw();
    }

    void WidgetProject::Paste()
    { 
        if (!_selections.CopiedSceneObject && !_selections.CopiedComponent)
            return;

        if (!_selections.ClickedSceneObject)
            _selections.ClickedSceneObject = Editor::Instance().GetSceneRoot().GetInternalPtr();

        if (_selections.CopiedComponent)
        {
            UINT32 type = _selections.CopiedComponent->GetCoreType();

            switch (type)
            {
            case TID_CCamera:
                {
                    HCamera component = _selections.ClickedSceneObject->AddComponent<CCamera>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            case TID_CCameraFlyer:
                {
                    HCameraFlyer component = _selections.ClickedSceneObject->AddComponent<CCameraFlyer>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            case TID_CCameraUI:
                {
                    HCameraUI component = _selections.ClickedSceneObject->AddComponent<CCameraUI>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            case TID_CLight:
                {
                    HLight component = _selections.ClickedSceneObject->AddComponent<CLight>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            case TID_CRenderable:
                {
                    HRenderable component = _selections.ClickedSceneObject->AddComponent<CRenderable>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            case TID_CSkybox:
                {
                    if (SceneManager::Instance().FindComponents<CSkybox>().size() > 0)
                        break;

                    HSkybox component = _selections.ClickedSceneObject->AddComponent<CSkybox>();
                    component->Clone(_selections.CopiedComponent->GetHandle());
                    component->Initialize();
                }
                break;

            default:
                break;
            }
        }
        else if (_selections.CopiedSceneObject)
        {
            if (_selections.CopiedSceneObject == Editor::Instance().GetSceneRoot().GetInternalPtr())
                return;

            HSceneObject sceneObject = SceneObject::Create("SceneObject");
            sceneObject->Clone(_selections.CopiedSceneObject);

            _selections.ClickedSceneObject->GetHandle();
            sceneObject->SetParent(_selections.ClickedSceneObject->GetHandle());
        }

        Editor::Instance().NeedsRedraw();
    }

    void WidgetProject::Delete()
    { 
        if (!_selections.ClickedSceneObject && !_selections.ClickedComponent)
            return;

        if (_selections.ClickedComponent)
        {
            if (_selections.CopiedComponent == _selections.ClickedComponent)
                _selections.CopiedComponent = nullptr;

            _selections.ClickedComponent->GetSceneObject()->DestroyComponent(_selections.ClickedComponent.get(), true);
            _selections.ClickedComponent = nullptr;
        }
        else if (_selections.ClickedSceneObject)
        {
            if (_selections.ClickedSceneObject == Editor::Instance().GetSceneRoot().GetInternalPtr())
                return;

            if (_selections.CopiedSceneObject == _selections.ClickedSceneObject)
                _selections.CopiedSceneObject = nullptr;

            _selections.ClickedSceneObject->Destroy(true);
            _selections.ClickedSceneObject = nullptr;
        }

        Editor::Instance().NeedsRedraw();
    }

    String WidgetProject::GetComponentIcon(const HComponent& component)
    {
        String title;
        UINT32 type = component->GetCoreType();

        switch (type)
        {
        case TID_Component:
            title = ICON_FA_SHAPES + String(" ");
            break;
        case TID_CCamera:
            title = ICON_FA_CAMERA + String(" ");
            break;
        case TID_CRenderable:
            title = ICON_FA_OBJECT_GROUP + String(" ");
            break;
        case TID_CLight:
            title = ICON_FA_LIGHTBULB + String(" ");
            break;
        case TID_CSkybox:
            title = ICON_FA_GLOBE + String(" ");
            break;
        case TID_CCameraFlyer:
            title = ICON_FA_CAMERA + String(" ");
            break;
        case TID_CCameraUI:
            title = ICON_FA_CAMERA + String(" ");
            break;
        default:
            break;
        }

        title += component->GetName();
        return title;
    }
}
 