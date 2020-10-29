#pragma once

#include "TeCorePrerequisites.h"
#include "Widget/TeWidget.h"
#include "Utility/TeModule.h"
#include <vector>
#include <memory>

namespace te
{
    class Editor : public Module<Editor>
    {
    public:
        enum class WindowType
        {
            Script, Viewport, Resources, Console, Game
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

        /** Called to inform the editor that some element has been modified and viewport must be updated */
        void NeedsRedraw();

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

    protected:
        struct EditorSettings
        {
            SPtr<Widget> WMenuBar   = nullptr;
            SPtr<Widget> WToolbar   = nullptr;
            SPtr<Widget> WProject   = nullptr;
            SPtr<Widget> WViewport  = nullptr;
            SPtr<Widget> WScript    = nullptr;
            SPtr<Widget> WResources = nullptr;
            SPtr<Widget> WConsole   = nullptr;
            SPtr<Widget> WGame      = nullptr;
            const char* EditorName  = "Editor";
            bool Show = true;
        };

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
        std::vector<SPtr<Widget>> _widgets;
        EditorSettings _settings;
        bool _editorBegun;

        HSceneObject _viewportSO;
        HSceneObject _sceneSO;

        HCamera _viewportCamera;
        HCameraUI _viewportCameraUI;
        HSceneObject _viewportCameraSO;
        
        HCamera _uiCamera;
        HSceneObject _uiCameraSO;

        SelectionData _selections;

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // TODO Temp for debug purpose
        HMesh _loadedMeshMonkey;
        HMesh _loadedMeshPlane;
        HTexture _loadedTextureMonkey;
        HTexture _loadedCubemapTexture;

        HMaterial _materialMonkey;

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
}
