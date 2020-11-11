#pragma once

#include "TeCorePrerequisites.h"
#include "Widget/TeWidget.h"
#include "Utility/TeModule.h"
#include "ImGuiExt/TeImGuiFileBrowser.h"

#include <vector>
#include <memory>

namespace te
{
    class GpuPicking;
    class Selection;
    class Hud;

    class Editor : public Module<Editor>
    {
    public:
        enum class EditorState
        {
            Modified, Saved
        };

        enum class WindowType
        {
            Project,
            RenderOptions,
            Console,
            Resources,
            Viewport,
            Script,
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
            SPtr<Widget> WRenderOptions = nullptr;
            SPtr<Widget> WConsole = nullptr;
            SPtr<Widget> WResources = nullptr;
            SPtr<Widget> WViewport = nullptr;
            SPtr<Widget> WScript = nullptr;
            SPtr<Widget> WMaterials = nullptr;
            SPtr<Widget> WProperties = nullptr;

            SPtr<Widget> WMaterial = nullptr;
            const char* EditorName = "Editor";
            bool Show = true;

            EditorState State = EditorState::Modified;
            String FilePath;
        };

    public:
        TE_MODULE_STATIC_HEADER_MEMBER(Editor)

        Editor();
        ~Editor();

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

        /** In order to handle selection in 3D viewport, we need to call gpuPicking system in order to generate an update to date render texture */
        void NeedsGpuPicking(UINT32 x, UINT32 y);

        /** If we need a redraw or if 3D viewport size change, we need to call this method to force picking render */
        void MakeGpuPickingDirty();

        /** If something has changed, we need to redraw hud elements such as cameras and lights on top of render */
        void MakeHudDirty();

        /** If we need a redraw or if 3D viewport size change, we need to call this method to force selected elements render */
        void MakeGpuSelectionDirty();

        /** Get viewport camera handle */
        HCamera& GetViewportCamera() { return _viewportCamera; }

        /** Get viewport camera UI handle */
        HCameraUI& GetViewportCameraUI() { return _viewportCameraUI;  }

        /** Get scene root object */
        HSceneObject& GetSceneRoot() { return _sceneSO; }

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

        /** Set the current preview viewport camera */
        void SetPreviewViewportCamera(HCamera& camera) { _previewViewportCamera = camera.GetNewHandleFromExisting(); }

        /** Save current scene */
        void Save();

        /** Open current scene */
        void Open();

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

    protected:
        bool _editorBegun;

        std::vector<SPtr<Widget>> _widgets;

        ImGuiFileBrowser _fileBrowser;
        SelectionData _selections;
        EditorSettings _settings;

        // viewport scene object is a hidden node to store stuff like cameras not visible
        HSceneObject _viewportSO;
        // root node for the user
        HSceneObject _sceneSO;
        
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
        UPtr<GpuPicking> _gpuPicking;
        bool _gpuPickingDirty;

        // If something has changed, we need to redraw hud elements such as cameras and lights on top of render
        UPtr<Hud> _hud;
        bool _hudDirty;

        // Current selected renderables, cameras and lights will be higglighted
        UPtr<Selection> _gpuSelection;
        bool _gpuSelectionDirty;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // TODO Temp for debug purpose
        HMesh _loadedMeshMonkey;
        HMesh _loadedMeshPlane;
        HTexture _loadedTextureMonkey;
        HTexture _loadedCubemapTexture;
        HTexture _loadedPlaneTexture;

        HMaterial _monkeyMaterial;
        HMaterial _planeMaterial;

        HSceneObject _sceneRenderableMonkeySO;
        HSceneObject _sceneRenderablePlaneSO;
        HSceneObject _sceneLightSO;
        HSceneObject _sceneSkyboxSO;

        HLight _light;
        HSkybox _skybox;
        HRenderable _renderableMonkey;
        HRenderable _renderablePlane;
#endif
    };

    /**	Provides easy access to Editor. */
    Editor& gEditor();
}
