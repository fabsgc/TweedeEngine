#include "TeWidgetProject.h"

#include "Resources/TeResourceManager.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Animation/TeAnimation.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"
#include "Components/TeCAnimation.h"
#include "Components/TeCBone.h"
#include "Components/TeCAudioSource.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCMeshSoftBody.h"
#include "Components/TeCEllipsoidSoftBody.h"
#include "Components/TeCRopeSoftBody.h"
#include "Components/TeCPatchSoftBody.h"
#include "Components/TeCConeTwistJoint.h"
#include "Components/TeCD6Joint.h"
#include "Components/TeCHingeJoint.h"
#include "Components/TeCSliderJoint.h"
#include "Components/TeCSphericalJoint.h"
#include "Components/TeCBoxCollider.h"
#include "Components/TeCPlaneCollider.h"
#include "Components/TeCSphereCollider.h"
#include "Components/TeCCylinderCollider.h"
#include "Components/TeCCapsuleCollider.h"
#include "Components/TeCMeshCollider.h"
#include "Components/TeCConeCollider.h"
#include "Components/TeCHeightFieldCollider.h"
#include "Components/TeCDecal.h"
#include "../TeEditorUtils.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-security"  // warning: format string is not a string literal (potentially insecure)
#endif

namespace te
{
    WidgetProject::WidgetProject()
        : Widget(WidgetType::Project)
        , _selections(gEditor().GetSelectionData())
        , _expandToSelection(false)
        , _expandDragToSelection(false)
        , _expandedToSelection(false)
        , _handleSelectionWindowSwitch(false)
    { 
        _title = PROJECT_TITLE;
        _flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    WidgetProject::~WidgetProject()
    { }

    void WidgetProject::Initialize()
    {
        _deleteBtn = VirtualButton(EditorUtils::DELETE_BINDING);
        _copyBtn = VirtualButton(EditorUtils::COPY_BINDING);
        _pasteBtn = VirtualButton(EditorUtils::PASTE_BINDING);
    }

    void WidgetProject::Update()
    {
        if (gEditor().IsEditorRunning())
        {
            HSceneObject& sceneSO = gEditor().GetRunningSceneRoot();
            ShowTree(sceneSO);
        }
        else
        {
            HSceneObject& sceneSO = gEditor().GetSceneRoot();
            ShowTree(sceneSO);
        }
        
        return;
    }

    void WidgetProject::UpdateBackground()
    { }

    void WidgetProject::ShowTree(HSceneObject& sceneObject)
    {
        OnTreeBegin();
        ShowSceneObjectTree(sceneObject, true);

        // If we have been expanding to show an entity and no more expansions are taking place, we reached it.
        // So, we stop expanding and we bring it into view.
        if (_expandToSelection && !_expandedToSelection)
        {
            ImGui::ScrollToBringRectIntoView(_window, _selectedSceneObjectRect);

            // If we had a drag event manage during previous showtree, we want to keep expandToSelection 
            // value to expand selection to the new dragged position object
            if (!_expandDragToSelection)
                _expandToSelection = false;

            _expandDragToSelection = false;
        }

        OnTreeEnd();
    }

    void WidgetProject::ShowSceneObjectTree(HSceneObject& sceneObject, bool expand)
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

        ImGui::PushID((int)sceneObjectId);
        {
            const bool isNodeOpened = ImGui::TreeNodeEx(
                reinterpret_cast<void*>(static_cast<intptr_t>(sceneObjectId)), nodeFlags, nodeTitle.c_str());

            // Keep a copy of the selected item's rect so that we can scroll to bring it into view
            if ((nodeFlags & ImGuiTreeNodeFlags_Selected) && _expandToSelection)
                _selectedSceneObjectRect = _window->DC.LastItemRect;

            // Manually detect some useful states
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
                _selections.HoveredSceneObject = sceneObject.GetInternalPtr();

            // Handle drag and drop
            HandleDragAndDrop(sceneObject);

            // Recursively show all child nodes
            if (isNodeOpened)
            {
                if (components.size() > 0)
                    ShowComponentsTree(sceneObject);

                for (auto& child : children)
                    ShowSceneObjectTree(child);

                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }

    void WidgetProject::ShowComponentsTree(HSceneObject& sceneObject)
    {
        auto components = sceneObject->GetComponents();
        _expandedToSelection = false;

        for (auto& component : components)
        {
            UINT64 componentId = component->GetInstanceId();
            ImGuiTreeNodeFlags componentFlags =
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_Bullet |
                ImGuiTreeNodeFlags_Leaf;

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
                        //ImGui::SetNextItemOpen(true);
                        _expandedToSelection = true;
                    }
                }
            }

            String componentIcon = GetComponentIcon(component);
            if (_selections.ClickedComponent == component.GetInternalPtr())
                componentIcon += String("  ") + ICON_FA_CARET_RIGHT;

            ImGui::PushID((int)componentId);
            {
                const bool isNodeOpened = ImGui::TreeNodeEx(
                    reinterpret_cast<void*>(static_cast<intptr_t>(componentId)), componentFlags, componentIcon.c_str());

                // Keep a copy of the selected item's rect so that we can scroll to bring it into view
                if ((componentFlags & ImGuiTreeNodeFlags_Selected) && _expandToSelection)
                    _selectedSceneObjectRect = _window->DC.LastItemRect;

                // Manually detect some useful states
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
                {
                    _selections.HoveredComponent = component.GetInternalPtr();
                }

                // Handle drag and drop
                HandleDragAndDrop(component);

                if(isNodeOpened)
                    ImGui::TreePop();
            }
            ImGui::PopID();
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
        HandleSelectionWindowSwitch();
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
        if (rightClick)
        {
            if (!gEditor().IsEditorRunning())
            {
                if (_selections.HoveredSceneObject)
                    SetSelectedSceneObject(_selections.HoveredSceneObject);

                if (_selections.HoveredComponent)
                    SetSelectedComponent(_selections.HoveredComponent);

                ImGui::OpenPopup("##ProjectContextMenu");
            }
        }

        // Clicking on empty space - Clear selection
        if ((leftClick || rightClick) && !_selections.HoveredComponent && !_selections.HoveredSceneObject)
        {
            _selections.ClickedSceneObject = nullptr;
            _selections.ClickedComponent = nullptr;

            gEditor().NeedsRedraw();
        }
    }

    void WidgetProject::HandleKeyShortcuts()
    {
        if (gEditor().IsEditorRunning())
            return;

        if (!ImGui::IsWindowHovered())
            return;

        if (gVirtualInput().IsButtonDown(_copyBtn))
        {
            if (_selections.ClickedComponent)
                _selections.CopiedComponent = _selections.ClickedComponent;
            else
                _selections.CopiedSceneObject = _selections.ClickedSceneObject;

            gEditor().NeedsRedraw();
        }

        if (gVirtualInput().IsButtonDown(_pasteBtn))
            Paste();

        if (gVirtualInput().IsButtonDown(_deleteBtn))
            Delete();
    }

    void WidgetProject::HandleSelectionWindowSwitch()
    {
        auto& io = ImGui::GetIO();
        if (_selections.ClickedComponent && _handleSelectionWindowSwitch && !io.MouseDown[ImGuiMouseButton_Left])
        {
            switch (_selections.ClickedComponent->GetCoreType())
            {
            case TID_CCamera:
            case TID_CCameraUI:
            case TID_CCameraFlyer:
            case TID_CRenderable:
            case TID_CLight:
            case TID_CSkybox:
            case TID_CAudioListener:
            case TID_CAudioSource:
            case TID_CRigidBody:
            case TID_CMeshSoftBody:
            case TID_CEllipsoidSoftBody:
            case TID_CRopeSoftBody:
            case TID_CPatchSoftBody:
            case TID_CConeTwistJoint:
            case TID_CD6Joint:
            case TID_CHingeJoint:
            case TID_CSliderJoint:
            case TID_CSphericalJoint:
            case TID_CBoxCollider:
            case TID_CPlaneCollider:
            case TID_CSphereCollider:
            case TID_CCylinderCollider:
            case TID_CCapsuleCollider:
            case TID_CMeshCollider:
            case TID_CConeCollider:
            case TID_CHeightFieldCollider:
            case TID_CDecal:
                gEditor().PutFocus(Editor::WindowType::Viewport);
                break;

            case TID_CScript:
                gEditor().PutFocus(Editor::WindowType::Script);
                break;
            }

            _handleSelectionWindowSwitch = false;
        }
    }

    void WidgetProject::HandleDragAndDrop(HSceneObject& sceneObject)
    {
        if (gEditor().IsEditorRunning())
            return;

        HSceneObject& root = gEditor().GetSceneRoot();

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            _dragPayload.Type = DragPayloadType::SceneObject;
            _dragPayload.Uuid = sceneObject->GetUUID();
            CreateDragPayload(_dragPayload);
            ImGui::Text(sceneObject->GetName().c_str());
            ImGui::EndDragDropSource();
        }

        if (auto payload = ReceiveDragPayload(DragPayloadType::SceneObject))
        {
            if (root->GetUUID() != payload->Uuid)
            {
                HSceneObject currentSO = root->GetSceneObject(payload->Uuid, true);

                if (!currentSO.Empty())
                {
                    currentSO->SetParent(sceneObject, false);
                    currentSO->SetActiveHierarchy(sceneObject->GetActive());

                    _selections.ClickedSceneObject = currentSO.GetInternalPtr();
                    _selections.ClickedComponent = nullptr;

                    _expandToSelection = true;
                    _expandDragToSelection = true;

                    // ugly but best way to update all children
                    sceneObject->Move(Vector3::ZERO);

                    gEditor().NeedsRedraw();
                    gEditor().GetSettings().State = Editor::EditorState::Modified;
                }
            }
        }

        if (auto payload = ReceiveDragPayload(DragPayloadType::Component))
        {
            HComponent currentCO = root->GetComponent(payload->Uuid, true);

            if (!currentCO.Empty())
            {
                HSceneObject oldSceneObject = currentCO->GetSceneObject();
                bool oldParentActive = currentCO->GetSceneObject()->GetActive();
                bool newParentActive = sceneObject->GetActive();

                currentCO->GetSceneObject()->RemoveComponent(currentCO);
                currentCO->SetSceneObject(sceneObject);
                sceneObject->AddExistingComponent(currentCO);

                _selections.ClickedSceneObject = sceneObject.GetInternalPtr();
                _selections.ClickedComponent = currentCO.GetInternalPtr();

                if (oldParentActive != newParentActive)
                {
                    if(newParentActive == false)
                        gSceneManager().NotifyComponentDeactivated(currentCO, true);
                    else
                        gSceneManager().NotifyComponentActivated(currentCO, true);
                }

                _expandToSelection = true;
                _expandDragToSelection = true;

                // if we've moved an animation call RestoreInternal()
                if (currentCO->GetCoreType() == TID_CAnimation)
                    static_object_cast<CAnimation>(currentCO)->Initialize();

                // if we've moved a renderable call RestoreInternal on both new and old sceneObject if there is an animation
                if (currentCO->GetCoreType() == TID_CRenderable)
                {
                    HAnimation animOldSO = static_object_cast<CAnimation>(oldSceneObject->GetComponent(TID_CAnimation));
                    if (!animOldSO.Empty())
                        animOldSO->Initialize();

                    HAnimation animNewSO = static_object_cast<CAnimation>(sceneObject->GetComponent(TID_CAnimation));
                    if (!animNewSO.Empty())
                        animNewSO->Initialize();
                }

                // if we've moved a bone, we need to trigger potential old and new animations to update their states
                if (currentCO->GetCoreType() == TID_CBone)
                    currentCO->Initialize();

                // if we've moved an audioListener or an audioSource, we need to trigger RestoreInternal()
                if (currentCO->GetCoreType() == TID_CAudioListener || currentCO->GetCoreType() == TID_CAudioSource)
                    currentCO->Initialize();

                // if we've moved a rigidBody call OnEnabled()
                if (currentCO->GetCoreType() == TID_CRigidBody)
                {
                    HRigidBody rigidBody = static_object_cast<CRigidBody>(currentCO);
                    rigidBody->Initialize();
                    rigidBody->SetLinkedSO(currentCO->SO());
                }

                // if we've moved a softBody call OnEnabled()
                if (currentCO->GetCoreType() == TID_CMeshSoftBody)
                {
                    HMeshSoftBody softBody = static_object_cast<CMeshSoftBody>(currentCO);
                    softBody->Initialize();
                    softBody->SetLinkedSO(currentCO->SO());
                }
                if (currentCO->GetCoreType() == TID_CEllipsoidSoftBody)
                {
                    HEllipsoidSoftBody softBody = static_object_cast<CEllipsoidSoftBody>(currentCO);
                    softBody->Initialize();
                    softBody->SetLinkedSO(currentCO->SO());
                }
                if (currentCO->GetCoreType() == TID_CRopeSoftBody)
                {
                    HRopeSoftBody softBody = static_object_cast<CRopeSoftBody>(currentCO);
                    softBody->Initialize();
                    softBody->SetLinkedSO(currentCO->SO());
                }
                if (currentCO->GetCoreType() == TID_CPatchSoftBody)
                {
                    HPatchSoftBody softBody = static_object_cast<CPatchSoftBody>(currentCO);
                    softBody->Initialize();
                    softBody->SetLinkedSO(currentCO->SO());
                }

                // if we've moved a collider call RestoreInternal()
                if (currentCO->GetCoreType() == TID_CBoxCollider)
                    static_object_cast<CBoxCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CPlaneCollider)
                    static_object_cast<CPlaneCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CSphereCollider)
                    static_object_cast<CSphereCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CCylinderCollider)
                    static_object_cast<CCylinderCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CCapsuleCollider)
                    static_object_cast<CCapsuleCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CMeshCollider)
                    static_object_cast<CMeshCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CConeCollider)
                    static_object_cast<CConeCollider>(currentCO)->Initialize();
                if (currentCO->GetCoreType() == TID_CHeightFieldCollider)
                    static_object_cast<CHeightFieldCollider>(currentCO)->Initialize();

                // ugly but best way to update all children
                sceneObject->Move(Vector3::ZERO);

                gEditor().NeedsRedraw();
                gEditor().GetSettings().State = Editor::EditorState::Modified;
            }
        }
    }

    void WidgetProject::HandleDragAndDrop(HComponent& component)
    {
        if (gEditor().IsEditorRunning())
            return;

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            _dragPayload.Type = DragPayloadType::Component;
            _dragPayload.Uuid = component->GetUUID();
            CreateDragPayload(_dragPayload);
            ImGui::Text(component->GetName().c_str());
            ImGui::EndDragDropSource();
        }
    }

    void WidgetProject::CreateDragPayload(const DragDropPayload& payload)
    {
        ImGui::SetDragDropPayload(reinterpret_cast<const char*>(&payload.Type), 
            reinterpret_cast<const void*>(&payload), sizeof(payload), ImGuiCond_Once);
    }

    WidgetProject::DragDropPayload* WidgetProject::ReceiveDragPayload(DragPayloadType type)
    {
        DragDropPayload* payload = nullptr;

        if (ImGui::BeginDragDropTarget())
        {
            if (const auto payload_imgui = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
                payload = static_cast<DragDropPayload*>(payload_imgui->Data);

            ImGui::EndDragDropTarget();
        }

        return payload;
    }

    void WidgetProject::SetSelectedSceneObject(SPtr<SceneObject> sceneObject)
    {
        _selections.ClickedSceneObject = sceneObject;
        _selections.ClickedComponent = nullptr;
        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;

        gEditor().NeedsRedraw();
    }

    void WidgetProject::SetSelectedComponent(SPtr<Component> component)
    {
        _selections.ClickedComponent = component;
        _selections.ClickedSceneObject = component->GetSceneObject().GetInternalPtr();
        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;

        gEditor().NeedsRedraw();
    }

    void WidgetProject::Popups()
    {
        PopupContextMenu();
    }

    void WidgetProject::PopupContextMenu()
    {
        if (gEditor().IsEditorRunning())
            return;

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
                            CreateAudioSource();
                        if (ImGui::MenuItem(ICON_FA_HEADPHONES " Audio listener"))
                            CreateAudioListener();

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu(ICON_FA_BOXES " Physics"))
                    {
                        if (ImGui::MenuItem(ICON_FA_BOXES " Rigid Body"))
                            CreateRigidBody();
                        if (ImGui::BeginMenu(ICON_FA_BOXES " Soft Body"))
                        {
                            if (ImGui::MenuItem(ICON_FA_BOXES " Mesh Soft Body"))
                                CreateSoftBody(TID_CMeshSoftBody);
                            if (ImGui::MenuItem(ICON_FA_BOXES " Ellipsoid Soft Body"))
                                CreateSoftBody(TID_CEllipsoidSoftBody);
                            if (ImGui::MenuItem(ICON_FA_BOXES " Rope Soft Body"))
                                CreateSoftBody(TID_CRopeSoftBody);
                            if (ImGui::MenuItem(ICON_FA_BOXES " Patch Soft Body"))
                                CreateSoftBody(TID_CPatchSoftBody);

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu(ICON_FA_LINK " Joint"))
                        {
                            if (ImGui::MenuItem(ICON_FA_LINK " Cone Twist Joint"))
                                CreateJoint(TID_CConeTwistJoint);
                            if (ImGui::MenuItem(ICON_FA_LINK " D6 Joint"))
                                CreateJoint(TID_CD6Joint);
                            if (ImGui::MenuItem(ICON_FA_LINK " Hinge Joint"))
                                CreateJoint(TID_CHingeJoint);
                            if (ImGui::MenuItem(ICON_FA_LINK " Slider Joint"))
                                CreateJoint(TID_CSliderJoint);
                            if (ImGui::MenuItem(ICON_FA_LINK " Spherical Joint"))
                                CreateJoint(TID_CSphericalJoint);

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu(ICON_FA_CUBE " Collider"))
                        {
                            if (ImGui::MenuItem(ICON_FA_CUBE " Box Collider"))
                                CreateCollider(TID_CBoxCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Plane Collider"))
                                CreateCollider(TID_CPlaneCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Sphere Collider"))
                                CreateCollider(TID_CSphereCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Cylinder Collider"))
                                CreateCollider(TID_CCylinderCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Capsule Collider"))
                                CreateCollider(TID_CCapsuleCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Mesh Collider"))
                                CreateCollider(TID_CMeshCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Cone Collider"))
                                CreateCollider(TID_CMeshCollider);
                            if (ImGui::MenuItem(ICON_FA_CUBE " Terrain Collider"))
                                CreateCollider(TID_CHeightFieldCollider);

                            ImGui::EndMenu();
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::MenuItem(ICON_FA_STEP_FORWARD " Animation"))
                        CreateAnimation();
                    if (ImGui::MenuItem(ICON_FA_GLOBE " Skybox"))
                        CreateSkybox();
                    if (ImGui::MenuItem(ICON_FA_SCROLL " Script"))
                        CreateScript();
                    if (ImGui::MenuItem(ICON_FA_BONE " Bone"))
                        CreateBone();
                    if (ImGui::MenuItem(ICON_FA_IMAGE " Decal"))
                        CreateDecal();

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
        _handleSelectionWindowSwitch = true;

        _selections.ClickedSceneObject = sceneObject.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateRenderable(RenderableType type)
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CRenderable>().Empty())
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
        _handleSelectionWindowSwitch = true;

        _selections.ClickedComponent = renderable.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
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
        _handleSelectionWindowSwitch = true;

        _selections.ClickedComponent = light.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateCamera(TypeID_Core type)
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        switch (type)
        {
            case TID_CCamera:
            {
                if (!_selections.ClickedSceneObject->GetComponent<CCamera>().Empty())
                    return;

                HCamera camera = _selections.ClickedSceneObject->AddComponent<CCamera>();
                camera->SetName("Camera");
                camera->Initialize();
                _selections.ClickedComponent = camera.GetInternalPtr();
            }
            break;

            case TID_CCameraFlyer:
            {
                if (!_selections.ClickedSceneObject->GetComponent<CCameraFlyer>().Empty())
                    return;

                HCameraFlyer camera = _selections.ClickedSceneObject->AddComponent<CCameraFlyer>();
                camera->SetName("Flying Camera");
                camera->Initialize();
                _selections.ClickedComponent = camera.GetInternalPtr();
            }
            break;

            case TID_CCameraUI:
            {
                if (!_selections.ClickedSceneObject->GetComponent<CCameraUI>().Empty())
                    return;

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
        _handleSelectionWindowSwitch = true;

        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateAudioSource()
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HAudioSource audioSource = _selections.ClickedSceneObject->AddComponent<CAudioSource>();
        audioSource.Get()->SetName("Audio source");
        audioSource.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = audioSource.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateAudioListener()
    { 
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HAudioListener audioListener = _selections.ClickedSceneObject->AddComponent<CAudioListener>();
        audioListener.Get()->SetName("Audio listener");
        audioListener.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = audioListener.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateScript()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CScript>().Empty())
            return;

        HScript script = _selections.ClickedSceneObject->AddComponent<CScript>();
        script.Get()->SetName("Script");
        script.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = script.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateAnimation()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CAnimation>().Empty())
            return;

        HAnimation animation = _selections.ClickedSceneObject->AddComponent<CAnimation>();
        animation.Get()->SetName("Animation");
        animation.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = animation.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateBone()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        HBone bone = _selections.ClickedSceneObject->AddComponent<CBone>();
        bone.Get()->SetName("Bone");
        bone.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = bone.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
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
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = skybox.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateRigidBody()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CRigidBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CMeshSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CEllipsoidSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CRopeSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CPatchSoftBody>().Empty())
        {
            return;
        }

        HRigidBody rigidBody = _selections.ClickedSceneObject->AddComponent<CRigidBody>();
        rigidBody.Get()->SetName("Rigid Body");
        rigidBody.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        _selections.ClickedComponent = rigidBody.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateSoftBody(TypeID_Core type)
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CRigidBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CMeshSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CEllipsoidSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CRopeSoftBody>().Empty() ||
            !_selections.ClickedSceneObject->GetComponent<CPatchSoftBody>().Empty())
        {
            return;
        }

        switch (type)
        {
            case TID_CMeshSoftBody:
            {
                HMeshSoftBody softBody = _selections.ClickedSceneObject->AddComponent<CMeshSoftBody>();
                softBody.Get()->SetName("Mesh Soft Body");
                softBody.Get()->Initialize();
                _selections.ClickedComponent = softBody.GetInternalPtr();
            }
            break;

            case TID_CEllipsoidSoftBody:
            {
                HEllipsoidSoftBody softBody = _selections.ClickedSceneObject->AddComponent<CEllipsoidSoftBody>();
                softBody.Get()->SetName("Ellipsoid Soft Body");
                softBody.Get()->Initialize();
                _selections.ClickedComponent = softBody.GetInternalPtr();
            }
            break;

            case TID_CRopeSoftBody:
            {
                HRopeSoftBody softBody = _selections.ClickedSceneObject->AddComponent<CRopeSoftBody>();
                softBody.Get()->SetName("Rope Soft Body");
                softBody.Get()->Initialize();
                _selections.ClickedComponent = softBody.GetInternalPtr();
            }
            break;

            case TID_CPatchSoftBody:
            {
                HPatchSoftBody softBody = _selections.ClickedSceneObject->AddComponent<CPatchSoftBody>();
                softBody.Get()->SetName("Patch Soft Body");
                softBody.Get()->Initialize();
                _selections.ClickedComponent = softBody.GetInternalPtr();
            }
            break;

            default:
            break;
        }

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
        
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateJoint(TypeID_Core type)
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        switch (type)
        {
            case TID_CConeTwistJoint:
            {
                HConeTwistJoint joint = _selections.ClickedSceneObject->AddComponent<CConeTwistJoint>();
                joint.Get()->SetName("Cone Twist Joint");
                joint.Get()->Initialize();
                _selections.ClickedComponent = joint.GetInternalPtr();
            }
            break;

            case TID_CD6Joint:
            {
                HD6Joint joint = _selections.ClickedSceneObject->AddComponent<CD6Joint>();
                joint.Get()->SetName("D6 Joint");
                joint.Get()->Initialize();
                _selections.ClickedComponent = joint.GetInternalPtr();
            }
            break;

            case TID_CHingeJoint:
            {
                HHingeJoint joint = _selections.ClickedSceneObject->AddComponent<CHingeJoint>();
                joint.Get()->SetName("Hinge Joint");
                joint.Get()->Initialize();
                _selections.ClickedComponent = joint.GetInternalPtr();
            }
            break;

            case TID_CSliderJoint:
            {
                HSliderJoint joint = _selections.ClickedSceneObject->AddComponent<CSliderJoint>();
                joint.Get()->SetName("Slider Joint");
                joint.Get()->Initialize();
                _selections.ClickedComponent = joint.GetInternalPtr();
            }
            break;

            case TID_CSphericalJoint:
            {
                HSphericalJoint joint = _selections.ClickedSceneObject->AddComponent<CSphericalJoint>();
                joint.Get()->SetName("Spherical Joint");
                joint.Get()->Initialize();
                _selections.ClickedComponent = joint.GetInternalPtr();
            }
            break;

            default:
            break;
        }
        
        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;

        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateCollider(TypeID_Core type)
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        switch (type)
        {
            case TID_CBoxCollider:
            {
                HBoxCollider collider = _selections.ClickedSceneObject->AddComponent<CBoxCollider>();
                collider.Get()->SetName("Box Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CPlaneCollider:
            {
                HPlaneCollider collider = _selections.ClickedSceneObject->AddComponent<CPlaneCollider>();
                collider.Get()->SetName("Plane Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CSphereCollider:
            {
                HSphereCollider collider = _selections.ClickedSceneObject->AddComponent<CSphereCollider>();
                collider.Get()->SetName("Sphere Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CCylinderCollider:
            {
                HCylinderCollider collider = _selections.ClickedSceneObject->AddComponent<CCylinderCollider>();
                collider.Get()->SetName("Cylinder Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CCapsuleCollider:
            {
                HCapsuleCollider collider = _selections.ClickedSceneObject->AddComponent<CCapsuleCollider>();
                collider.Get()->SetName("Capsule Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CMeshCollider:
            {
                HMeshCollider collider = _selections.ClickedSceneObject->AddComponent<CMeshCollider>();
                collider.Get()->SetName("Mesh Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CConeCollider:
            {
                HConeCollider collider = _selections.ClickedSceneObject->AddComponent<CConeCollider>();
                collider.Get()->SetName("Cone Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            case TID_CHeightFieldCollider:
            {
                HHeightFieldCollider collider = _selections.ClickedSceneObject->AddComponent<CHeightFieldCollider>();
                collider.Get()->SetName("Terrain Collider");
                collider.Get()->Initialize();
                _selections.ClickedComponent = collider.GetInternalPtr();
            }
            break;

            default:
            break;
        }

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;

        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::CreateDecal()
    {
        if (!_selections.ClickedSceneObject || _selections.ClickedComponent)
            return;

        if (!_selections.ClickedSceneObject->GetComponent<CDecal>().Empty())
            return;

        HDecal decal = _selections.ClickedSceneObject->AddComponent<CDecal>();
        decal.Get()->SetName("Decal");
        decal.Get()->Initialize();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;

        _selections.ClickedComponent = decal.GetInternalPtr();
        gEditor().NeedsRedraw();
        gEditor().GetSettings().State = Editor::EditorState::Modified;
    }

    void WidgetProject::Paste()
    { 
        gEditor().Paste();

        _expandToSelection = true;
        _handleSelectionWindowSwitch = true;
    }

    void WidgetProject::Delete()
    { 
        gEditor().Delete();
    }

    String WidgetProject::GetComponentIcon(const HComponent& component)
    {
        String title = ((component->SO()->GetActive()) ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
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
        case TID_CAnimation:
            title += String("  ") + ICON_FA_STEP_FORWARD;
            break;
        case TID_CBone:
            title += String("  ") + ICON_FA_BONE;
            break;
        case TID_CAudioListener:
            title += String("  ") + ICON_FA_HEADPHONES;
            break;
        case TID_CAudioSource:
            title += String("  ") + ICON_FA_MICROPHONE;
            break;
        case TID_CRigidBody:
        case TID_CMeshSoftBody:
        case TID_CEllipsoidSoftBody:
        case TID_CRopeSoftBody:
        case TID_CPatchSoftBody:
            title += String("  ") + ICON_FA_BOXES;
            break;
        case TID_CConeTwistJoint:
        case TID_CD6Joint:
        case TID_CHingeJoint:
        case TID_CSliderJoint:
        case TID_CSphericalJoint:
            title += String("  ") + ICON_FA_LINK;
            break;
        case TID_CBoxCollider:
        case TID_CPlaneCollider:
        case TID_CSphereCollider:
        case TID_CCylinderCollider:
        case TID_CCapsuleCollider:
        case TID_CMeshCollider:
        case TID_CConeCollider:
        case TID_CHeightFieldCollider:
            title += String("  ") + ICON_FA_CUBE;
            break;
        case TID_CDecal:
            title += String("  ") + ICON_FA_IMAGE;
            break;
        default:
            title += String("  ") + ICON_FA_QUESTION_CIRCLE;
            break;
        }

        title = title + String("  ") + component->GetName();
        return title;
    }
}
 