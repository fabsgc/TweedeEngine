#include "TeEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include "Widget/TeWidgetMenuBar.h"
#include "Widget/TeWidgetToolBar.h"
#include "Widget/TeWidgetProject.h"
#include "Widget/TeWidgetProperties.h"
#include "Widget/TeWidgetProfiler.h"
#include "Widget/TeWidgetSettings.h"
#include "Widget/TeWidgetRenderOptions.h"
#include "Widget/TeWidgetConsole.h"
#include "Widget/TeWidgetViewport.h"
#include "Widget/TeWidgetResources.h"
#include "Widget/TeWidgetScript.h"
#include "Widget/TeWidgetMaterials.h"
#include "Widget/TeWidgetShaders.h"

#include "Gui/TeGuiAPI.h"
#include "TeCoreApplication.h"
#include "Renderer/TeCamera.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCScript.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCAnimation.h"
#include "Components/TeCBone.h"
#include "Components/TeCAudioSource.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCMeshSoftBody.h"
#include "Components/TeCEllipsoidSoftBody.h"
#include "Components/TeCRopeSoftBody.h"
#include "Components/TeCPatchSoftBody.h"
#include "Components/TeCMeshSoftBody.h"
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
#include "Components/TeCAnimation.h"
#include "Scene/TeSceneManager.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeBuiltinResources.h"
#include "TeEditorResManager.h"
#include "Mesh/TeMesh.h"
#include "Math/TeVector2I.h"
#include "Image/TeColor.h"
#include "Image/TePixelUtil.h"
#include "TeEditorUtils.h"
#include "Physics/TePhysics.h"

#include "Selection/TeEditorPicking.h"
#include "Selection/TeSelection.h"
#include "Selection/TeHud.h"

#include "RenderAPI/TeRenderTarget.h"
#include "Renderer/TeRendererUtility.h"

// TODO Temp for debug purpose
#include "Importer/TeImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Audio/TeAudioClipImportOptions.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Threading/TeTaskScheduler.h"
#include "Utility/TeTime.h"

#ifndef GImGui
ImGuiContext* GImGui = NULL;
#endif

namespace te
{
    TE_MODULE_STATIC_MEMBER(Editor)

    const Vector<String> Editor::TexturesExtensions = { ".png", ".jpeg", ".jpg", ".dds", ".tiff", ".tif", ".tga", ".bmp" };
    const Vector<String> Editor::MeshesExtensions = { ".obj", ".dae", ".fbx", ".stl", ".gltf" };
    const Vector<String> Editor::SoundsExtensions = { ".ogg", ".wav", ".flac" };

    const String Editor::TexturesExtensionsStr = ".png,.jpeg,.jpg,.dds,.tiff,.tif,.tga,.bmp";
    const String Editor::MeshesExtensionsStr = ".obj,.dae,.fbx,.stl,.gltf";
    const String Editor::SoundsExtensionsStr = ".ogg,.flac,.wav";

    const Vector<UINT32> Editor::ComponentsWhichNeedGuizmo = {
        TID_CRenderable,
        TID_CRigidBody,
        TID_CMeshSoftBody,
        TID_CEllipsoidSoftBody,
        TID_CRopeSoftBody,
        TID_CPatchSoftBody,
        TID_CLight,
        TID_CCamera,
        TID_CCameraFlyer,
        TID_CAudioSource,
        TID_CAudioListener,
        TID_CBoxCollider,
        TID_CCapsuleCollider,
        TID_CConeCollider,
        TID_CCylinderCollider,
        TID_CHeightFieldCollider,
        TID_CMeshCollider,
        TID_CPlaneCollider,
        TID_CSphereCollider,
        TID_CConeTwistJoint,
        TID_CD6Joint,
        TID_CHingeJoint,
        TID_CSliderJoint,
        TID_CSphericalJoint
    };

    Editor::Editor()
        : _editorBegun(false)
        , _pickingDirty(true)
        , _selectionDirty(true)
        , _hudDirty(true)
        , _physicsDirty(true)
        , _animationDebug(true)
        , _guizmoState(ImGuizmoState::Active)
        , _guizmoOperation(ImGuizmo::OPERATION::TRANSLATE)
        , _guizmoMode(ImGuizmo::MODE::WORLD)
    { }

    Editor::~Editor()
    {
        _widgets.clear();
    }

    void Editor::OnStartUp()
    {
        EditorResManager::StartUp();

        gCoreApplication().GetState().SetFlag(ApplicationState::Mode::Game, false);
        gCoreApplication().GetState().SetFlag(ApplicationState::Mode::Physics, false);
        gCoreApplication().GetState().SetFlag(ApplicationState::Mode::Scripting, false);
        gCoreApplication().GetState().SetFlag(ApplicationState::Mode::Animation, false);

        InitializeInput();
        InitializeScene();
        InitializeUICamera();
        InitializeViewportCamera();

        _picking = te_unique_ptr_new<EditorPicking>();
        _selection = te_unique_ptr_new<Selection>();
        _hud = te_unique_ptr_new<Hud>();

        _picking->Initialize();
        _selection->Initialize();
        _hud->Initialize();

        if (GuiAPI::Instance().IsGuiInitialized())
            InitializeGui();
    }

    void Editor::OnShutDown()
    { 
        EditorResManager::ShutDown();
    }

    void Editor::Update()
    {
        if (GuiAPI::Instance().IsGuiInitialized())
        {
            GuiAPI::Instance().BeginFrame();
            GuiAPI::Instance().Update();

            ImGuizmo::BeginFrame();

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
                BeginGui();

            for (auto widget : _widgets)
            {
                if (widget->BeginGui())
                {
                    widget->Update();
                    widget->EndGui();
                }
                else
                {
                    widget->UpdateBackground();
                }
            }

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
                EndGui();
        }

        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Mode::Game);
        if (isRunning)
        {
            HSceneObject sphereSO = _runningSceneSO->GetSceneObject("Sphere", true);

            //if(!sphereSO.Empty())
            //    sphereSO->Rotate(Vector3(0.0f, 0.5f, 0.0f), Radian(2.5f * gTime().GetFrameDelta()));
        }
    }

    void Editor::PostRender()
    {
        if (_previewViewportCamera->GetRenderSettings()->OverlayOnly)
            return;

        if (_hudDirty && _previewViewportCamera == _viewportCamera) // only for default camera
            _hud->Render(_previewViewportCamera, _sceneSO);

        if (_selectionDirty && _previewViewportCamera == _viewportCamera) // only for default camera
        {
            RendererUtility::RenderTextureData& viewportData =
                static_cast<WidgetViewport*>(&*_settings.WViewport)->GetRenderTextureData();
            _selection->Render(_previewViewportCamera, viewportData);
        }

        if (_physicsDirty && _previewViewportCamera == _viewportCamera) // only for default camera
        {
            gPhysics().DrawDebug(_previewViewportCamera->GetInternal(), _previewViewportCamera->GetViewport()->GetTarget());
        }

        _hudDirty = false;
        _selectionDirty = false;
        _physicsDirty = false;
    }

    void Editor::NeedsRedraw()
    {
        if (!_settings.WViewport)
            return;

        static_cast<WidgetViewport*>(&*_settings.WViewport)->NeedsRedraw();
        MakePickingDirty();
        MakeSelectionDirty();
        MakeHudDirty();
        MakePhysicsDirty();
    }

    void Editor::NeedsPicking(UINT32 x, UINT32 y)
    {
        if (!_settings.WViewport || _previewViewportCamera->GetRenderSettings()->OverlayOnly)
            return;

        if (!ImGuizmo::IsUsing())
        {
            if (_pickingDirty)
            {
                RendererUtility::RenderTextureData viewportData =
                    static_cast<WidgetViewport*>(&*_settings.WViewport)->GetRenderTextureData();
                Picking::RenderParam pickingData(viewportData.Width, viewportData.Height);

                _picking->ComputePicking(_previewViewportCamera, pickingData, _sceneSO);
                _pickingDirty = false;
            }

            SPtr<GameObject> gameObject = _picking->GetGameObjectAt(x, y);
            if (gameObject)
            {
                bool selectableSceneObject = (_selections.ClickedSceneObject && _selections.ClickedSceneObject->HasComponent(ComponentsWhichNeedGuizmo));

                if (!ImGuizmo::IsOver() || (!_selections.ClickedComponent && !selectableSceneObject))
                {
                    SPtr<Component> component = std::static_pointer_cast<Component>(gameObject);

                    _selections.ClickedComponent = component;
                    _selections.ClickedSceneObject = component->GetSceneObject().GetInternalPtr();

                    if (_settings.WProject)
                        std::static_pointer_cast<WidgetProject>(_settings.WProject)->ForceExpandToSelection();
                }
            }
            else
            {
                if (!_selections.ClickedComponent ||
                    _selections.ClickedComponent->GetCoreType() == TID_CRenderable ||
                    _selections.ClickedComponent->GetCoreType() == TID_CCamera ||
                    _selections.ClickedComponent->GetCoreType() == TID_CLight ||
                    _selections.ClickedComponent->GetCoreType() == TID_CAudioListener ||
                    _selections.ClickedComponent->GetCoreType() == TID_CRigidBody ||
                    _selections.ClickedComponent->GetCoreType() == TID_CEllipsoidSoftBody ||
                    _selections.ClickedComponent->GetCoreType() == TID_CMeshSoftBody ||
                    _selections.ClickedComponent->GetCoreType() == TID_CPatchSoftBody ||
                    _selections.ClickedComponent->GetCoreType() == TID_CRopeSoftBody ||
                    _selections.ClickedComponent->GetCoreType() == TID_CBoxCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CCapsuleCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CConeCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CMeshCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CPlaneCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CSphereCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CHeightFieldCollider ||
                    _selections.ClickedComponent->GetCoreType() == TID_CConeTwistJoint ||
                    _selections.ClickedComponent->GetCoreType() == TID_CD6Joint ||
                    _selections.ClickedComponent->GetCoreType() == TID_CHingeJoint ||
                    _selections.ClickedComponent->GetCoreType() == TID_CSliderJoint ||
                    _selections.ClickedComponent->GetCoreType() == TID_CSphericalJoint ||
                    _selections.ClickedComponent->GetCoreType() == TID_CAudioSource)
                {
                    if (!ImGuizmo::IsOver())
                    {
                        _selections.ClickedComponent = nullptr;
                        _selections.ClickedSceneObject = nullptr;
                    }
                }
            }
        }

        NeedsRedraw();
    }

    void Editor::MakePickingDirty()
    {
        _pickingDirty = true;
    }

    void Editor::MakeHudDirty()
    {
        _hudDirty = true;
    }

    void Editor::MakeSelectionDirty()
    {
        _selectionDirty = true;
    }

    void Editor::MakePhysicsDirty()
    {
        _physicsDirty = true;
    }

    void Editor::InitializeInput()
    {
        auto inputConfig = gVirtualInput().GetConfiguration();

        inputConfig->RegisterButton(WidgetMenuBar::NEW_BINDING, TE_N, ButtonModifier::Ctrl);
        inputConfig->RegisterButton(WidgetMenuBar::OPEN_BINDING, TE_O, ButtonModifier::Ctrl);
        inputConfig->RegisterButton(WidgetMenuBar::SAVE_BINDING, TE_S, ButtonModifier::Ctrl);
        inputConfig->RegisterButton(WidgetMenuBar::SAVE_AS_BINDING, TE_S, ButtonModifier::ShiftCtrl);
        inputConfig->RegisterButton(WidgetMenuBar::QUIT_BINDING, TE_A, ButtonModifier::Ctrl);
        inputConfig->RegisterButton(WidgetMenuBar::LOAD_RESOURCE_BINDING, TE_R, ButtonModifier::Ctrl);

        inputConfig->RegisterButton(EditorUtils::DELETE_BINDING, TE_DELETE);
        inputConfig->RegisterButton(EditorUtils::COPY_BINDING, TE_C, ButtonModifier::Ctrl);
        inputConfig->RegisterButton(EditorUtils::PASTE_BINDING, TE_V, ButtonModifier::Ctrl);
        
        inputConfig->RegisterButton(CCameraUI::ROTATE_BINDING, TE_MOUSE_RIGHT);
        inputConfig->RegisterButton(CCameraUI::MOVE_BINDING, TE_LSHIFT);
        inputConfig->RegisterButton(CCameraUI::ZOOM_BINDING, TE_LCONTROL);

        inputConfig->RegisterAxis(CCameraUI::HORIZONTAL_AXIS_BINDING, VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
        inputConfig->RegisterAxis(CCameraUI::VERTICAL_AXIS_BINDING, VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
        inputConfig->RegisterAxis(CCameraUI::SCROLL_AXIS_BINDING, VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseZ));

        inputConfig->RegisterButton(WidgetViewport::RETARGET_BINDING, TE_DECIMAL);
        inputConfig->RegisterButton(WidgetViewport::PICKING_BINDING, TE_MOUSE_LEFT);
    }

    void Editor::InitializeScene()
    {
        _sceneSO = SceneObject::Create("Scene");
        LoadScene();
    }

    void Editor::InitializeUICamera()
    {
        _uiCameraSO = SceneObject::Create("UICamera");
        _uiCamera = _uiCameraSO->AddComponent<CCamera>();
        _uiCamera->GetViewport()->SetClearColorValue(Color(0.2f, 0.2f, 0.2f, 1.0f));
        _uiCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _uiCamera->SetMain(true);
        _uiCamera->SetPriority(1);
        _uiCamera->Initialize();

        _uiCamera->SetNearClipDistance(5);
        _uiCamera->SetFarClipDistance(10000);
        _uiCamera->SetLayers(0);

        SPtr<RenderSettings> settings = _uiCamera->GetRenderSettings();
        settings->OverlayOnly = true;
        _uiCamera->SetRenderSettings(settings); // renderer is not updated otherwise

        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
    }

    void Editor::InitializeViewportCamera()
    {
        _viewportSO = SceneObject::Create("UIViewport");

        _viewportCameraSO = SceneObject::Create("UIViewportCamera");
        _viewportCameraSO->SetParent(_viewportSO);

        _viewportCameraSO->SetPosition(Vector3(3.0f, 1.25f, 0.0f));
        _viewportCameraSO->LookAt(Vector3(0.0f, 0.0f, 0.0f));

        _viewportCamera = _viewportCameraSO->AddComponent<CCamera>();
        _viewportCamera->GetViewport()->SetClearColorValue(Color(0.42f, 0.67f, 0.94f, 1.0f));
        _viewportCamera->Initialize();
        _viewportCamera->SetMSAACount(gCoreApplication().GetWindow()->GetDesc().MultisampleCount);
        _viewportCamera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _viewportCamera->SetName("Viewport camera");

        _viewportCameraUI = _viewportCameraSO->AddComponent<CCameraUI>();
        _viewportCameraUI->Initialize();
        _viewportCameraUI->SetTarget(Vector3(0.0f, 0.0f, 0.0f));
        _viewportCameraUI->SetName("Viewport camera UI");

        auto settings = _viewportCamera->GetRenderSettings();
        settings->MotionBlur.Enabled = false;
        settings->ScreenSpaceReflections.Enabled = false;
        _viewportCamera->SetRenderSettings(settings); // renderer is not updated otherwise

        _previewViewportCamera = _viewportCamera;
    }

    void Editor::InitializeGui()
    {
        //Context already created in Gui plugin, we need to have the same pointed value in our local GimGui
        GImGui = ImGui::GetCurrentContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigWindowsResizeFromEdges = true;
        io.ConfigViewportsNoTaskBarIcon = true;
        ApplyStyleGui();

        _widgets.emplace_back(te_unique_ptr_new<WidgetMenuBar>()); _settings.WMenuBar = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetToolBar>()); _settings.WToolbar = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetProject>()); _settings.WProject = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetProperties>()); _settings.WProperties = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetProfiler>()); _settings.WProfiler = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetSettings>()); _settings.WSettings = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetRenderOptions>()); _settings.WRenderOptions = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetConsole>()); _settings.WConsole = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetScript>()); _settings.WScript = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetShaders>()); _settings.WShaders = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetMaterials>()); _settings.WMaterial = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetResources>()); _settings.WResources = _widgets.back();
        _widgets.emplace_back(te_unique_ptr_new<WidgetViewport>()); _settings.WViewport = _widgets.back();

        _settings.WResources->SetVisible(false);
        _settings.WConsole->SetVisible(false);

        for (auto& widget : _widgets)
            widget->Initialize();
    }

    void Editor::ApplyStyleGui() const
    {
        {
            // Color settings    
            const auto color_text = ImVec4(0.810f, 0.810f, 0.810f, 1.000f);
            const auto color_text_disabled = ImVec4(color_text.x, color_text.y, color_text.z, 0.5f);
            const auto color_interactive = ImVec4(0.338f, 0.338f, 0.338f, 1.000f);
            const auto color_interactive_hovered = ImVec4(0.450f, 0.450f, 0.450f, 1.000f);
            const auto color_interactive_clicked = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
            const auto color_background = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
            const auto color_background_content = ImVec4(35.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f);
            const auto color_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

            // Use default dark style as a base
            ImGui::StyleColorsDark();

            // Colors
            ImVec4* colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = color_text;
            colors[ImGuiCol_TextDisabled] = color_text_disabled;
            colors[ImGuiCol_WindowBg] = color_background;            // Background of normal windows
            colors[ImGuiCol_ChildBg] = color_background;             // Background of child windows
            colors[ImGuiCol_PopupBg] = color_background;             // Background of popups, menus, tooltips windows
            colors[ImGuiCol_Border] = color_interactive;
            colors[ImGuiCol_BorderShadow] = color_shadow;
            colors[ImGuiCol_FrameBg] = color_background_content;     // Background of checkbox, radio button, plot, slider, text input
            colors[ImGuiCol_FrameBgHovered] = color_interactive;
            colors[ImGuiCol_FrameBgActive] = color_interactive_clicked;
            colors[ImGuiCol_TitleBg] = color_background_content;
            colors[ImGuiCol_TitleBgActive] = color_interactive;
            colors[ImGuiCol_TitleBgCollapsed] = color_background;
            colors[ImGuiCol_MenuBarBg] = color_background_content;
            colors[ImGuiCol_ScrollbarBg] = color_background_content;
            colors[ImGuiCol_ScrollbarGrab] = color_interactive;
            colors[ImGuiCol_ScrollbarGrabHovered] = color_interactive_hovered;
            colors[ImGuiCol_ScrollbarGrabActive] = color_interactive_clicked;
            colors[ImGuiCol_CheckMark] = color_text;
            colors[ImGuiCol_SliderGrab] = color_interactive;
            colors[ImGuiCol_SliderGrabActive] = color_interactive_clicked;
            colors[ImGuiCol_Button] = color_interactive;
            colors[ImGuiCol_ButtonHovered] = color_interactive_hovered;
            colors[ImGuiCol_ButtonActive] = color_interactive_clicked;
            colors[ImGuiCol_Header] = color_interactive;            // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
            colors[ImGuiCol_HeaderHovered] = color_interactive_hovered;
            colors[ImGuiCol_HeaderActive] = color_interactive_clicked;
            colors[ImGuiCol_Separator] = color_interactive;
            colors[ImGuiCol_SeparatorHovered] = color_interactive_hovered;
            colors[ImGuiCol_SeparatorActive] = color_interactive_clicked;
            colors[ImGuiCol_ResizeGrip] = color_interactive;
            colors[ImGuiCol_ResizeGripHovered] = color_interactive_hovered;
            colors[ImGuiCol_ResizeGripActive] = color_interactive_clicked;
            colors[ImGuiCol_Tab] = color_interactive;
            colors[ImGuiCol_TabHovered] = color_interactive_hovered;
            colors[ImGuiCol_TabActive] = color_interactive_clicked;
            colors[ImGuiCol_TabUnfocused] = color_interactive;
            colors[ImGuiCol_TabUnfocusedActive] = color_interactive;            // Might be called active, but it's active only because it's it's the only tab available, the user didn't really activate it
            colors[ImGuiCol_DockingPreview] = color_interactive_clicked;    // Preview overlay color when about to docking something
            colors[ImGuiCol_DockingEmptyBg] = color_interactive;            // Background color for empty node (e.g. CentralNode with no window docked into it)
            colors[ImGuiCol_PlotLines] = color_interactive;
            colors[ImGuiCol_PlotLinesHovered] = color_interactive_hovered;
            colors[ImGuiCol_PlotHistogram] = color_interactive;
            colors[ImGuiCol_PlotHistogramHovered] = color_interactive_hovered;
            colors[ImGuiCol_TextSelectedBg] = color_background;
            colors[ImGuiCol_DragDropTarget] = color_interactive_hovered;    // Color when hovering over target
            colors[ImGuiCol_NavHighlight] = color_background;             // Gamepad/keyboard: current highlighted item
            colors[ImGuiCol_NavWindowingHighlight] = color_background;             // Highlight window when using CTRL+TAB
            colors[ImGuiCol_NavWindowingDimBg] = color_background;             // Darken/colorize entire screen behind the CTRL+TAB window list, when active
            colors[ImGuiCol_ModalWindowDimBg] = color_background;             // Darken/colorize entire screen behind a modal window, when one is active

            // Spatial settings
            const auto fontSize = 14.0f;
            const auto roundness = 0.0f;

            // Spatial
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowBorderSize = 1.0f;
            style.FrameBorderSize = 0.0f;
            style.ScrollbarSize = 16.0f;
            style.FramePadding = ImVec2(6, 6);
            style.ItemSpacing = ImVec2(6, 6);
            style.WindowMenuButtonPosition = ImGuiDir_Right;
            style.WindowRounding = roundness;
            style.FrameRounding = roundness;
            style.PopupRounding = roundness;
            style.GrabRounding = roundness;
            style.ScrollbarRounding = roundness;
            style.Alpha = 1.0f;

            // Font
            auto& io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF("Data/Fonts/CalibriBold.ttf", fontSize);
            //io.FontGlobalScale = fontScale;

            ImFontConfig config;
            config.MergeMode = true;
            static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            io.Fonts->AddFontFromFileTTF("Data/Fonts/Fa-solid-900.ttf", fontSize, &config, icon_ranges);
        }
    }

    void Editor::BeginGui()
    {
        // Set window flags
        const auto window_flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoSavedSettings;

        // Set window position and size
        float offsetY = 0;
        offsetY += _settings.WMenuBar ? _settings.WMenuBar->GetHeight() : 0;
        offsetY += _settings.WToolbar ? _settings.WToolbar->GetHeight() : 0;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offsetY));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offsetY));
        ImGui::SetNextWindowViewport(viewport->ID);

        // Set window style
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.0f);

        // Begin window
        bool open = true;
        _editorBegun = ImGui::Begin(_settings.EditorName, &open, window_flags);
        ImGui::PopStyleVar(3);

        // Begin dock space
        if (_editorBegun)
        {
            // Dock space
            const auto windowId = ImGui::GetID(_settings.EditorName);
            if (!ImGui::DockBuilderGetNode(windowId))
            {
                // Reset current docking state
                ImGui::DockBuilderRemoveNode(windowId);
                ImGui::DockBuilderAddNode(windowId, ImGuiDockNodeFlags_None);
                ImGui::DockBuilderSetNodeSize(windowId, ImGui::GetMainViewport()->Size);

                ImGuiID dockMainId = windowId;

                ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.20f, nullptr, &dockMainId);
                ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.25f, nullptr, &dockMainId);
                ImGuiID dockBottomId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.305f, nullptr, &dockMainId);

                const ImGuiID dockLeftBottomId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Down, 0.6f, nullptr, &dockLeftId);
                const ImGuiID dockRightBottomId = ImGui::DockBuilderSplitNode(dockRightId, ImGuiDir_Down, 0.6f, nullptr, &dockRightId);

                // Dock windows
                ImGui::DockBuilderDockWindow(PROJECT_TITLE, dockLeftId);
                ImGui::DockBuilderDockWindow(PROFILER_TITLE, dockRightId);
                ImGui::DockBuilderDockWindow(SETTINGS_TITLE, dockRightId);
                ImGui::DockBuilderDockWindow(RENDER_OPTIONS_TITLE, dockRightId);
                ImGui::DockBuilderDockWindow(CONSOLE_TITLE, dockBottomId);
                ImGui::DockBuilderDockWindow(RESOURCES_TITLE, dockBottomId);
                ImGui::DockBuilderDockWindow(VIEWPORT_TITLE, dockMainId);
                ImGui::DockBuilderDockWindow(SCRIPT_TITLE, dockMainId);
                ImGui::DockBuilderDockWindow(SHADERS_TITLE, dockRightBottomId);
                ImGui::DockBuilderDockWindow(MATERIALS_TITLE, dockRightBottomId);
                ImGui::DockBuilderDockWindow(PROPERTIES_TITLE, dockLeftBottomId);
                ImGui::DockBuilderFinish(dockMainId);
            }

            ImGui::DockSpace(windowId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        }
    }

    void Editor::EndGui()
    {
        if (_editorBegun)
            ImGui::End();
    }

    Widget* Editor::GetWidget(Widget::WidgetType type)
    {
        for (auto& widget : _widgets)
        {
            if (widget->GetType() == type)
                return widget.get();
        }

        return nullptr;
    }

    void Editor::PutFocus(WindowType type) const
    {
        switch (type)
        {
        case WindowType::Project:
            _settings.WProject->PutFocus();
            break;
        case WindowType::RenderOptions:
            _settings.WRenderOptions->PutFocus();
            break;
        case WindowType::Console:
            _settings.WConsole->PutFocus();
            break;
        case WindowType::Resources:
            _settings.WResources->PutFocus();
            break;
        case WindowType::Viewport:
            _settings.WViewport->PutFocus();
            break;
        case WindowType::Script:
            _settings.WScript->PutFocus();
            break;
        case WindowType::Shaders:
            _settings.WShaders->PutFocus();
            break;
        case WindowType::Materials:
            _settings.WMaterials->PutFocus();
            break;
        case WindowType::Properties:
            _settings.WProperties->PutFocus();
            break;
        case WindowType::Profiler:
            _settings.WProfiler->PutFocus();
            break;
        case WindowType::Settings:
            _settings.WSettings->PutFocus();
            break;
        }
    }

    void GetComponentsFromTransform(const Transform& transform, float* matrixTranslation, float* matrixRotation, float* matrixScale)
    {
        Radian x, y, z;
        transform.GetRotation().ToEulerAngles(x, y, z);
        Vector3 position = transform.GetPosition();
        Vector3 rotation(x.ValueDegrees(), y.ValueDegrees(), z.ValueDegrees());
        Vector3 scale = transform.GetScale();

        matrixTranslation[0] = position.x;
        matrixTranslation[1] = position.y;
        matrixTranslation[2] = position.z;

        matrixRotation[0] = rotation.x;
        matrixRotation[1] = rotation.y;
        matrixRotation[2] = rotation.z;

        matrixScale[0] = scale.x;
        matrixScale[1] = scale.y;
        matrixScale[2] = scale.z;
    }

    void Editor::HandleImGuizmo()
    {
        bool somethingSelected = false;

        if (IsEditorRunning())
            return;

        if (_selections.ClickedComponent)
        {
            somethingSelected = true;
        }
        else if(_selections.ClickedSceneObject)
        {
            if (_selections.ClickedSceneObject->HasComponent(ComponentsWhichNeedGuizmo))
                somethingSelected = true;
        }

        if (somethingSelected && _guizmoState == ImGuizmoState::Active && _previewViewportCamera == _viewportCamera)
        {
            Transform transform;
            const float* proj = nullptr;
            const float* view = nullptr;
            float matrixTranslation[3];
            float matrixRotation[3];
            float matrixScale[3];
            float worldMatrix[4][4];
            float deltaWorldMatrix[4][4];
            float snap[3] = { 0.1f, 0.1f, 0.1f };

            ImGuizmo::SetDrawlist();

            // Different snap for rotation
            if (_guizmoOperation == ImGuizmo::OPERATION::ROTATE)
                snap[0] = 2.5f;

            // Retrieves View and Projection Matrix
            const Matrix4& viewMatrix = _previewViewportCamera->GetViewMatrix().Transpose();
            const Matrix4& projectionMatrix = _previewViewportCamera->GetProjectionMatrixRS().Transpose();
            viewMatrix.GetAsFloat(view);
            projectionMatrix.GetAsFloat(proj);

            // Retrieves WorldMatrix
            if (_selections.ClickedSceneObject)
                transform = _selections.ClickedSceneObject->GetTransform();
            else if (_selections.ClickedComponent)
                transform = _selections.ClickedComponent->SO()->GetTransform();
            else
                return;

            GetComponentsFromTransform(transform, matrixTranslation, matrixRotation, matrixScale);
            ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrix[0][0]);

            // Guizmo rendering
            ImGuizmo::Manipulate(view, proj, _guizmoOperation, _guizmoMode, &worldMatrix[0][0], &deltaWorldMatrix[0][0], &snap[0]);

            // Transform update
            if (ImGuizmo::IsUsing())
            {
                ImGuizmo::DecomposeMatrixToComponents(&worldMatrix[0][0], matrixTranslation, matrixRotation, matrixScale);

                switch (_guizmoOperation)
                {
                case ImGuizmo::OPERATION::TRANSLATE:
                {
                    ImGuizmo::DecomposeMatrixToComponents(&deltaWorldMatrix[0][0], matrixTranslation, matrixRotation, matrixScale);
                    Vector3 translation(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
                    if(!Math::ApproxEquals(translation, Vector3::ZERO))
                        _selections.ClickedSceneObject->Move(translation);
                }
                break;

                case ImGuizmo::OPERATION::SCALE:
                {
                    Vector3 scale(matrixScale[0], matrixScale[1], matrixScale[2]);
                    if (!Math::ApproxEquals(scale, Vector3::ZERO))
                        _selections.ClickedSceneObject->SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));
                }
                break;

                case ImGuizmo::OPERATION::ROTATE:
                {
                    Quaternion rotation;
                    ImGuizmo::DecomposeMatrixToComponents(&deltaWorldMatrix[0][0], matrixTranslation, matrixRotation, matrixScale);

                    Radian x(Degree((float)matrixRotation[0]));
                    Radian y(Degree((float)matrixRotation[1]));
                    Radian z(Degree((float)matrixRotation[2]));

                    if (!Math::ApproxEquals(matrixRotation[0], 0.0f) || !Math::ApproxEquals(matrixRotation[1], 0.0f) || !Math::ApproxEquals(matrixRotation[2], 0.0f))
                    {
                        rotation.FromEulerAngles(x, y, z);
                        _selections.ClickedSceneObject->Rotate(rotation);
                    }
                }
                break;

                default:
                    break;
                }

                NeedsRedraw();
            }
        }
    }

    void Editor::SetImGuizmoRect(const Vector2& position, const Vector2& size)
    {
        ImGuizmo::SetRect(position.x, position.y, size.x, size.y);
    }

    void Editor::SetImGuizmoState(ImGuizmoState state) 
    { 
        if (state == _guizmoState)
            return;

        _guizmoState = state;

        if (_guizmoState == ImGuizmoState::Active)
            ImGuizmo::Enable(true);
        else
            ImGuizmo::Enable(false);
    }

    void Editor::SetImGuizmoOperation(ImGuizmo::OPERATION operation)
    {
        _guizmoOperation = operation;
    }

    void Editor::SetImGuizmoMode(ImGuizmo::MODE mode)
    {
        _guizmoMode = mode;
    }

    void Editor::Save()
    {
        if (IsEditorRunning())
            return;

        _settings.State = EditorState::Saved;
        // TODO Save
    }

    void Editor::Open()
    {
        if (IsEditorRunning())
            return;

        _settings.State = EditorState::Saved;
        // TODO Open
    }

    void Editor::Paste()
    {
        if (IsEditorRunning())
            return;

        if (!_selections.CopiedSceneObject && !_selections.CopiedComponent)
            return;

        SPtr<SceneObject> clickedSceneObject = _selections.ClickedSceneObject ?
            _selections.ClickedSceneObject : GetSceneRoot().GetInternalPtr();

        if (_selections.CopiedComponent)
        {
            UINT32 type = _selections.CopiedComponent->GetComponentType();
            if (!clickedSceneObject->GetComponent(type).Empty())
                return;

            switch (_selections.CopiedComponent->GetCoreType())
            {
                case TID_CCamera:
                {
                    HCamera component = clickedSceneObject->AddComponent<CCamera>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CCameraFlyer:
                {
                    HCameraFlyer component = clickedSceneObject->AddComponent<CCameraFlyer>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CCameraUI:
                {
                    HCameraUI component = clickedSceneObject->AddComponent<CCameraUI>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CLight:
                {
                    SPtr<CLight> previousLight = std::static_pointer_cast<CLight>(_selections.CopiedComponent);
                    if (previousLight)
                    {
                        HLight component = clickedSceneObject->AddComponent<CLight>(previousLight->GetType());
                        component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                        component->Initialize();
                        _selections.ClickedComponent = component.GetInternalPtr();
                        _selections.CopiedComponent = component.GetInternalPtr();
                    }
                }
                break;

                case TID_CRenderable:
                {
                    HRenderable component = clickedSceneObject->AddComponent<CRenderable>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CScript:
                {
                    HScript component = clickedSceneObject->AddComponent<CScript>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
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
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CAnimation:
                {
                    HAnimation component = clickedSceneObject->AddComponent<CAnimation>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CBone:
                {
                    HBone component = clickedSceneObject->AddComponent<CBone>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CAudioListener:
                {
                    HAudioListener component = clickedSceneObject->AddComponent<CAudioListener>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CAudioSource:
                {
                    HAudioSource component = clickedSceneObject->AddComponent<CAudioSource>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CRigidBody:
                {
                    HRigidBody component = clickedSceneObject->AddComponent<CRigidBody>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CMeshSoftBody:
                {
                    HMeshSoftBody component = clickedSceneObject->AddComponent<CMeshSoftBody>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CEllipsoidSoftBody:
                {
                    HEllipsoidSoftBody component = clickedSceneObject->AddComponent<CEllipsoidSoftBody>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CRopeSoftBody:
                {
                    HRopeSoftBody component = clickedSceneObject->AddComponent<CRopeSoftBody>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CPatchSoftBody:
                {
                    HPatchSoftBody component = clickedSceneObject->AddComponent<CPatchSoftBody>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CConeTwistJoint:
                {
                    HConeTwistJoint component = clickedSceneObject->AddComponent<CConeTwistJoint>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CD6Joint:
                {
                    HD6Joint component = clickedSceneObject->AddComponent<CD6Joint>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CHingeJoint:
                {
                    HHingeJoint component = clickedSceneObject->AddComponent<CHingeJoint>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CSliderJoint:
                {
                    HSliderJoint component = clickedSceneObject->AddComponent<CSliderJoint>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CSphericalJoint:
                {
                    HSphericalJoint component = clickedSceneObject->AddComponent<CSphericalJoint>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CBoxCollider:
                {
                    HBoxCollider component = clickedSceneObject->AddComponent<CBoxCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CPlaneCollider:
                {
                    HPlaneCollider component = clickedSceneObject->AddComponent<CPlaneCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CSphereCollider:
                {
                    HSphereCollider component = clickedSceneObject->AddComponent<CSphereCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CCylinderCollider:
                {
                    HCylinderCollider component = clickedSceneObject->AddComponent<CCylinderCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CCapsuleCollider:
                {
                    HCapsuleCollider component = clickedSceneObject->AddComponent<CCapsuleCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CMeshCollider:
                {
                    HMeshCollider component = clickedSceneObject->AddComponent<CMeshCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CConeCollider:
                {
                    HConeCollider component = clickedSceneObject->AddComponent<CConeCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
                    component->Initialize();
                    _selections.ClickedComponent = component.GetInternalPtr();
                    _selections.CopiedComponent = component.GetInternalPtr();
                }
                break;

                case TID_CHeightFieldCollider:
                {
                    HHeightFieldCollider component = clickedSceneObject->AddComponent<CHeightFieldCollider>();
                    component->Clone(_selections.CopiedComponent->GetHandle(), " copy");
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
            if (_selections.CopiedSceneObject == GetSceneRoot().GetInternalPtr())
                return;

            HSceneObject sceneObject = SceneObject::Create("SceneObject");
            sceneObject->Clone(_selections.CopiedSceneObject, " copy");
            sceneObject->SetParent(clickedSceneObject->GetHandle());

            _selections.ClickedSceneObject = sceneObject.GetInternalPtr();
            _selections.CopiedSceneObject = sceneObject.GetInternalPtr();
        }

        NeedsRedraw();
        GetSettings().State = Editor::EditorState::Modified;
    }

    void Editor::Delete()
    {
        if (IsEditorRunning())
            return;

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
            if (_selections.ClickedSceneObject == GetSceneRoot().GetInternalPtr())
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

        NeedsRedraw();
        GetSettings().State = Editor::EditorState::Modified;
    }

    /** When editor goes to play mode, we create a copy of the current scene hierarchy and disable the original */
    void Editor::CreateRunningScene()
    {
        _sceneSO->SetActive(false);
        _runningSceneSO = SceneObject::Create("Scene");
        _runningSceneSO->Clone(_sceneSO);
        _runningSceneSO->SetActive(false);
        _runningSceneSO->SetActive(true);

        // Note : CJoint elements will not work when running the scene from the editor
        // because we can't easly associate cloned joint to cloned bodies
        // TODO : Find a way to associate joints to their bodies in the cloned scene
    }

    /** When editor goes to stop mode, we destroy to copied scene and restore the original one */
    void Editor::DestroyRunningScene()
    {
        _selections.ClickedComponent = nullptr;
        _selections.ClickedSceneObject = nullptr;
        _selections.HoveredComponent = nullptr;
        _selections.HoveredSceneObject = nullptr;

        _runningSceneSO->Destroy();
        _sceneSO->SetActive(true);
        NeedsRedraw();
    }

    void Editor::LoadScene()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        // IMPORT OPTIONS
        // ######################################################
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportCollisionShape = false;
        meshImportOptions->ImportMaterials = true;
        meshImportOptions->ImportTextures = false;
        meshImportOptions->ScaleFactor = 3.5f;
        meshImportOptions->ScaleSystemUnit = true;
        meshImportOptions->ImportZPrepassMesh = true;

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->GenerateMipsOnGpu = true;
        textureImportOptions->MipsPreserveCoverage = true;
        textureImportOptions->SRGB = true;

        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;
        textureCubeMapImportOptions->IsCubeMap = true;
        textureCubeMapImportOptions->SRGB = true;
        // ######################################################

        // LOAD MESH AND TEXTURES RESOURCES
        // ######################################################
        SPtr<MultiResource> MultiResourcesMesh = EditorResManager::Instance().LoadAll("Data/Meshes/VintageWooden/VintageWooden.obj", meshImportOptions);

        _sphereMesh = static_resource_cast<Mesh>(MultiResourcesMesh->Entries[0].Res);
        _zPrepassSphereMesh = static_resource_cast<ZPrepassMesh>(MultiResourcesMesh->Entries[1].Res);
        //_monkeyMesh = static_resource_cast<Mesh>(EditorResManager::Instance().LoadAll("Data/Meshes/Monkey/monkey-hd.obj", meshImportOptions)->Entries[0].Res);
        //_planeMesh = static_resource_cast<Mesh>(EditorResManager::Instance().LoadAll("Data/Meshes/Primitives/plane.obj", meshImportOptions)->Entries[0].Res);

        textureCubeMapImportOptions->Format = PixelUtil::BestFormatFromFile("Data/Textures/Skybox/skybox_night_512.png");
        _skyboxTexture = EditorResManager::Instance().Load<Texture>("Data/Textures/Skybox/skybox_night_512.png", textureCubeMapImportOptions);

        //textureImportOptions->Format = PixelUtil::BestFormatFromFile("Data/Textures/Cobble/diffuse1.jpg");
        //HTexture cobbleBaseColor = EditorResManager::Instance().Load<Texture>("Data/Textures/Cobble/diffuse1.jpg", textureImportOptions);

        //textureImportOptions->SRGB = false;
        //HTexture cobbleNormal = EditorResManager::Instance().Load<Texture>("Data/Textures/Cobble/normal1.jpg", textureImportOptions);
        //HTexture cobbleParallax = EditorResManager::Instance().Load<Texture>("Data/Textures/Cobble/parallax1.jpg", textureImportOptions);

        if (_sphereMesh.IsLoaded())
            _sphereMesh->SetName("Sphere Mesh");
        if (_zPrepassSphereMesh.IsLoaded())
            _zPrepassSphereMesh->SetName("Sphere Mesh");
        if (_sphereMesh.IsLoaded())
            _skyboxTexture->SetName("Skybox Texture");
        // ###################################################### 

        // GET BUILTIN OPAQUE SHADER
        // ######################################################
        HShader _shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);
        // ###################################################### 

        // SET MATERIALS
        // ######################################################
        {
            MaterialProperties monkeyMatprop;
            monkeyMatprop.BaseColor = Color(1.0f, 0.54f, 0.05f, 1.0f);
            monkeyMatprop.Metallic = 0.0f;
            monkeyMatprop.Roughness = 0.2f;
            //monkeyMatprop.UseBaseColorMap = true;
            //monkeyMatprop.UseNormalMap = true;
            //monkeyMatprop.UseParallaxMap = true;
            //monkeyMatprop.ParallaxScale = 0.05f;

            _sphereMaterial = Material::Create(_shader);
            _sphereMaterial->SetName("Sphere Material");
            _sphereMaterial->SetProperties(monkeyMatprop);
            //_monkeyMaterial->SetTexture("BaseColorMap", cobbleBaseColor);
            //_monkeyMaterial->SetTexture("NormalMap", cobbleNormal);
            //_monkeyMaterial->SetTexture("ParallaxMap", cobbleParallax);
        }
        // ######################################################

        // FILL SCENE WITH SKYBOX
        // ######################################################
        if (_skyboxTexture.IsLoaded())
        {
            _sceneSkyboxSO = SceneObject::Create("Skybox");
            _sceneSkyboxSO->SetParent(_sceneSO);
            _skybox = _sceneSkyboxSO->AddComponent<CSkybox>();
            _skybox->SetTexture(_skyboxTexture);
            _skybox->SetBrightness(1.f);
            _skybox->SetIBLIntensity(5000.f);
            _skybox->Initialize();
        }
        // ######################################################

        // FILL SCENE WITH DIRECTIONAL LIGHT
        // ######################################################
        _sceneLightSO = SceneObject::Create("Light");
        _sceneLightSO->SetParent(_sceneSO);
        _light = _sceneLightSO->AddComponent<CLight>(Light::Type::Directional);
        _light->Initialize();
        _light->SetIntensity(5000.f);
        _light->SetCastShadows(true);

        Quaternion rot;
        rot.FromEulerAngles(Radian(Degree(-21.69f)), Radian(Degree(36.4f)), Radian(Degree(15.06f)));
        _sceneLightSO->SetRotation(rot);
        _sceneLightSO->Move(Vector3(0.0f, 4.0f, 4.0f));
        // ######################################################

        // FILL SCENE WITH MESHES
        // ######################################################
        if (_sphereMesh.IsLoaded() && _sphereMaterial.IsLoaded())
        {
            _sceneRenderableSO = SceneObject::Create("Sphere");
            _sceneRenderableSO->SetParent(_sceneSO);
            _renderable = _sceneRenderableSO->AddComponent<CRenderable>();
            _renderable->SetMesh(_sphereMesh);
            _renderable->SetZPrepassMesh(_zPrepassSphereMesh);
            _renderable->SetMaterial(_sphereMaterial);
            _renderable->SetName("Renderable");
            _renderable->Initialize();
            _renderable->SetCastLight(true);
            _renderable->SetCastShadows(true);
            _renderable->SetReceiveShadows(true);
            _renderable->SetUseForZPrepass(true);
            _renderable->SetUseForLightProbes(true);
            _renderable->SetUseForLightProbes(true);

            _sceneRenderableSO->Rotate(Vector3::UNIT_Y, Radian(Math::HALF_PI));
            _sceneRenderableSO->Move(Vector3(0.0f, -1.0f, 0.0f));
        }

        EditorResManager::Instance().Add<Material>(_sphereMaterial);

        HShader shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Opaque);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Transparent);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::TransparentCullNone);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ZPrepass);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Blit);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Skybox);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::FXAA);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ToneMapping);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Bloom);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::MotionBlur);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::GaussianBlur);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Picking);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::HudPicking);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Selection);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::HudSelection);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::BulletDebug);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::SSAO);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::SSAOBlur);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::SSAODownSample);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::Decal);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::TextureDownsample);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::TextureCubeDownsample);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ReflectionCubeImportanceSample);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::IrradianceComputeSH);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::IrradianceReduceSH);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::IrradianceProjectSH);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::BlitSelection);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ShadowDepthNormal);
        EditorResManager::Instance().Add<Shader>(shader);
        shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ShadowDepthCube);
        EditorResManager::Instance().Add<Shader>(shader);
#endif
    }

    Editor& gEditor()
    {
        return Editor::Instance();
    }
}
