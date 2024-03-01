#pragma once

#include "TeCorePrerequisites.h"
#include "Widget/TeWidget.h"
#include "Utility/TeModule.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Components/TeCCamera.h"
#include "ImGuiExt/TeImGuiFileBrowser.h"
#include "MaterialsPreview/TeMaterialsPreview.h"

namespace te
{
    class Picking;
    class Selection;
    class Hud;
    class MaterialsPreview;

    class Editor : public Module<Editor>
    {
    public:
        enum class EditorState
        {
            Modified, Saved
        };

        enum class ImGuizmoState
        {
            Active, Inactive
        };

        enum class WindowType
        {
            Project,
            Profiler,
            Settings,
            RenderOptions,
            Console,
            Resources,
            Viewport,
            Script,
            Shaders,
            Materials,
            Properties
        };

        struct SelectionData
        {
            SPtr<SceneObject> HoveredSceneObject = nullptr;
            SPtr<SceneObject> ClickedSceneObject = nullptr;
            SPtr<SceneObject> CopiedSceneObject = nullptr;
            SPtr<Component> HoveredComponent = nullptr;
            SPtr<Component> ClickedComponent = nullptr;
            SPtr<Component> CopiedComponent = nullptr;

            ~SelectionData()
            {
                HoveredSceneObject = nullptr;
                ClickedSceneObject = nullptr;
                CopiedSceneObject = nullptr;
                HoveredComponent = nullptr;
                ClickedComponent = nullptr;
                CopiedComponent = nullptr;
            }
        };

        struct EditorSettings
        {
            SPtr<Widget> WMenuBar = nullptr;
            SPtr<Widget> WToolbar = nullptr;
            SPtr<Widget> WProject = nullptr;
            SPtr<Widget> WProfiler = nullptr;
            SPtr<Widget> WSettings = nullptr;
            SPtr<Widget> WRenderOptions = nullptr;
            SPtr<Widget> WConsole = nullptr;
            SPtr<Widget> WResources = nullptr;
            SPtr<Widget> WViewport = nullptr;
            SPtr<Widget> WScript = nullptr;
            SPtr<Widget> WMaterials = nullptr;
            SPtr<Widget> WProperties = nullptr;
            SPtr<Widget> WShaders = nullptr;

            const char* EditorName = "Editor";
            bool Show = true;

            EditorState State = EditorState::Modified;
        };

    public:
        Editor();
        virtual ~Editor();

        TE_MODULE_STATIC_HEADER_MEMBER(Editor)

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** Called every frame. */
        void Update();

        /** Called after render */
        void PostRender();

        /** Called to inform the editor that some element has been modified and viewport must be updated */
        void NeedsRedraw();

        /** In order to handle selection in 3D viewport, we need to call picking system in order to generate an update to render texture */
        void NeedsPicking(UINT32 x, UINT32 y);

        /** If we need a redraw or if 3D viewport size change, we need to call this method to force picking render */
        void MakePickingDirty();

        /** If something has changed, we need to redraw hud elements such as cameras and lights on top of render */
        void MakeHudDirty();

        /** If we need a redraw or if 3D viewport size change, we need to call this method to force selected elements to render */
        void MakeSelectionDirty();

        /** If we need a redraw or if 3D viewport size change, we need to call this method to force debug physics to render */
        void MakePhysicsDirty();

        /** Get viewport camera handle */
        HCamera& GetViewportCamera() { return _viewportCamera; }

        /** Get viewport camera UI handle */
        HCameraUI& GetViewportCameraUI() { return _viewportCameraUI;  }

        /** Get current project */
        HProject& GetProject() { return _project; }

        /** Get scene root object */
        HSceneObject& GetSceneRoot() { return _sceneSO; }

        /** Get running scene root object */
        HSceneObject& GetRunningSceneRoot() { return _runningSceneSO; }

        /** Get a widget pointer given its type. Return nullptr if not widget has been found */
        Widget* GetWidget(Widget::WidgetType type);

        /** Here we store all data on object selected using project tree or 3d view */
        SelectionData& GetSelectionData() { return _selections; }

        /** Depending on selections done wherever in the editor we could want to give focus to a window */
        void PutFocus(WindowType type) const;

        /** We can only have on instance of the file dialog */
        ImGuiFileBrowser& GetFileBrowser() { return _fileBrowser; }

        /** Some settings widgets can modify */
        EditorSettings& GetSettings() { return _settings; }

        /** Retrieves the current viewport camera. Can be the one created at editor initilization or one created by user in his scene */
        HCamera& GetPreviewViewportCamera() { return _previewViewportCamera; }

        /** Get raw pointer to MaterialsPreview instance */
        MaterialsPreview& GetMaterialsPreview() { return _materialsPreview; }

        /** Set the current preview viewport camera */
        void SetPreviewViewportCamera(HCamera& camera) { _previewViewportCamera = camera; }

        /** Manage ImGuizmo */
        void HandleImGuizmo();

        /** Update rendering rect of the Guizmo */
        void SetImGuizmoRect(const Vector2& position, const Vector2& size);

        /** Update Guizmo state (active/inactive */
        void SetImGuizmoState(ImGuizmoState state);

        /** Update Guizmo operations */
        void SetImGuizmoOperation(ImGuizmo::OPERATION operation);

        /** Update Guizmo mode */
        void SetImGuizmoMode(ImGuizmo::MODE mode);

        /** Get Guizmo operation */
        ImGuizmo::OPERATION GetImGuizmoOperation() const { return _guizmoOperation; }

        /** Get Guizmo state */
        ImGuizmoState GetImGuizmoState() const { return _guizmoState; }

        /** Get Guizmo mode */
        ImGuizmo::MODE GetImGuizmoMode() const { return _guizmoMode; }

        /** Is animation debug enabled */
        bool IsAnimationDebug() const { return _animationDebug; }

        /** Enable or disable animation debug */
        void SetAnimationDebug(bool debug) { _animationDebug = debug; }

        /** Create new project */
        bool NewProject(const String& path);

        /** Open an existing project */
        bool OpenProject(const String& path);

        /** Save current project */
        bool SaveProject(const String& path);

        /** Paste current selection */
        void Paste();

        /** Delete current selection */
        void Delete();

        /** When editor goes to play mode, we create a copy of the current scene hierarchy and disable the original */
        void CreateRunningScene();

        /** When editor goes to stop mode, we destroy to copied scene and restore the original one */
        void DestroyRunningScene();

        /** When loading a new scene, we want to remove the already exising scene */
        void DestroyScene();

        /** Check is editor is in play mode or not */
        bool IsEditorRunning() const { return gCoreApplication().GetState().IsFlagSet(ApplicationState::Mode::Game); }

    protected:
        void InitializeInput();
        void InitializeScene();
        void InitializeUICamera();
        void InitializeViewportCamera();
        void InitializeGui();
        void ApplyStyleGui() const;
        void BeginGui();
        void EndGui();

    protected:
        void LoadScene();
        void LoadEngineResources();

    public:
        static const Vector<String> TexturesExtensions;
        static const Vector<String> MeshesExtensions;
        static const Vector<String> SoundsExtensions;

        static const String TexturesExtensionsStr;
        static const String MeshesExtensionsStr;
        static const String SoundsExtensionsStr;

        static Event<void()> OnOpen;
        static Event<void()> OnSave;

    protected:
        static const Vector<UINT32> ComponentsWhichNeedGuizmo;

        bool _editorBegun;

        std::vector<SPtr<Widget>> _widgets;

        ImGuiFileBrowser _fileBrowser;
        SelectionData _selections;
        EditorSettings _settings;

        HProject _project;

        // viewport scene object is a hidden node to store stuff like cameras not visible
        HSceneObject _viewportSO;
        // root node for the user
        HSceneObject _sceneSO;
        HSceneObject _runningSceneSO;
        
        // Camera used to render GUI
        HCamera _uiCamera;
        HSceneObject _uiCameraSO;

        // Default camera for 3D viewport
        HCamera _viewportCamera;
        HCameraUI _viewportCameraUI;
        HSceneObject _viewportCameraSO;

        // we can use an user created camera for viewport;
        HCamera _previewViewportCamera;

        // I decided to use GPU Picking for 3D viewport selection handle
        // After NeedRedraws() or 3D viewport resize, we need to put this to true in order to force picking render
        UPtr<Picking> _picking;
        bool _pickingDirty;

        // Current selected renderables, cameras and lights will be higglighted
        UPtr<Selection> _selection;
        bool _selectionDirty;

        // If something has changed, we need to redraw hud elements such as cameras and lights on top of render
        UPtr<Hud> _hud;
        bool _hudDirty;

        // Using to store preview of all materials currently used
        MaterialsPreview _materialsPreview;

        // If something has changed, we need to redraw physics debug
        bool _physicsDirty;

        // Debug animation
        bool _animationDebug;

        // We only enable Guizmo if there is a viewport with the default camera enabled
        ImGuizmoState _guizmoState;
        ImGuizmo::OPERATION _guizmoOperation;
        ImGuizmo::MODE _guizmoMode;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // TODO Temp for debug purpose
        HMesh _sphereMesh;
        HZPrepassMesh _zPrepassSphereMesh;
        HTexture _skyboxTexture;
        HMaterial _sphereMaterial;

        HSceneObject _sceneRenderableSO;
        HSceneObject _sceneLightSO;
        HSceneObject _sceneSkyboxSO;

        HLight _light;
        HSkybox _skybox;
        HRenderable _renderable;
#endif
    };

    /**	Provides easy access to Editor. */
    Editor& gEditor();
}
