#include "TeRenderMan.h"
#include "Manager/TeRendererManager.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Renderer/TeCamera.h"
#include "Utility/TeTime.h"
#include "CoreUtility/TeCoreObjectManager.h"
#include "TeRenderCompositor.h"

namespace te
{
    RenderMan::RenderMan()
    { }

    RenderMan::~RenderMan()
    { }

    void RenderMan::Initialize()
    {
        Renderer::Initialize();

        _options = te_shared_ptr_new<RenderManOptions>();
        _scene = te_shared_ptr_new<RendererScene>(_options);

        _mainViewGroup = te_new<RendererViewGroup>(nullptr, 0);

        RenderCompositor::RegisterNodeType<RCNodeForwardPass>();
        RenderCompositor::RegisterNodeType<RCNodeFinalResolve>();
    }

    void RenderMan::Destroy()
    {
        Renderer::Destroy();
        _scene = nullptr;

        RenderCompositor::CleanUp();

        te_delete(_mainViewGroup);
    }

    void RenderMan::Update()
    { }

    const String& RenderMan::GetName() const
    {
        static String name = "RenderMan";
        return name;
    }

    void RenderMan::RenderAll()
    {
        CoreObjectManager::Instance().FrameSync();

        const SceneInfo& sceneInfo = _scene->GetSceneInfo();

        FrameTimings timings;
        timings.Time = gTime().GetTime();
        timings.TimeDelta = gTime().GetFrameDelta();
        timings.FrameIdx = gTime().GetFrameIdx();

        // Update global per-frame hardware buffers
        _scene->SetParamFrameParams(timings.Time);

        sceneInfo.RenderableReady.resize(sceneInfo.Renderables.size(), false);
        sceneInfo.RenderableReady.assign(sceneInfo.Renderables.size(), false);

        FrameInfo frameInfo(timings);

        // Update per-frame data for all renderable objects
        for (UINT32 i = 0; i < sceneInfo.Renderables.size(); i++)
            _scene->PrepareRenderable(i, frameInfo);

        // Gather all views
        for (auto& rtInfo : sceneInfo.RenderTargets)
        {
            Vector<RendererView*> views;
            SPtr<RenderTarget> target = rtInfo.Target;
            const Vector<Camera*>& cameras = rtInfo.Cameras;

            UINT32 numCameras = (UINT32)cameras.size();
            for (UINT32 i = 0; i < numCameras; i++)
            {
                UINT32 viewIdx = sceneInfo.CameraToView.at(cameras[i]);
                RendererView* viewInfo = sceneInfo.Views[viewIdx];
                views.push_back(viewInfo);
            }

            _mainViewGroup->SetViews(views.data(), (UINT32)views.size());
            _mainViewGroup->DetermineVisibility(sceneInfo);

            // Render everything
            bool anythingDrawn = RenderViews(*_mainViewGroup, frameInfo);

            if (rtInfo.Target->GetProperties().IsWindow && anythingDrawn)
            {
                RenderAPI::Instance().SwapBuffers(rtInfo.Target);
            }
        }
    }

    /** Renders all views in the provided view group. Returns true if anything has been draw to any of the views. */
    bool RenderMan::RenderViews(RendererViewGroup& viewGroup, const FrameInfo& frameInfo)
    {
        bool needs3DRender = false;
        UINT32 numViews = viewGroup.GetNumViews();
        for (UINT32 i = 0; i < numViews; i++)
        {
            RendererView* view = viewGroup.GetView(i);

            if (view->ShouldDraw3D())
            {
                needs3DRender = true;
                break;
            }
        }

        if (needs3DRender)
        {
            const SceneInfo& sceneInfo = _scene->GetSceneInfo();
            const VisibilityInfo& visibility = viewGroup.GetVisibilityInfo();

            // Update various buffers required by each renderable
            UINT32 numRenderables = (UINT32)sceneInfo.Renderables.size();
            for (UINT32 i = 0; i < numRenderables; i++)
            {
                if (!visibility.Renderables[i])
                    continue;

                _scene->PrepareVisibleRenderable(i, frameInfo);
            }
        }

        bool anythingDrawn = false;
        for (UINT32 i = 0; i < numViews; i++)
        {
            RendererView* view = viewGroup.GetView(i);

            if (!view->ShouldDraw())
            {
                continue;
            }

            const RenderSettings& settings = view->GetRenderSettings();
            if (settings.OverlayOnly)
            {
                if (RenderOverlay(*view, frameInfo))
                    anythingDrawn = true;
            }
            else
            {
                RenderSingleView(viewGroup, *view, frameInfo);
                anythingDrawn = true;
            }
        }

        return anythingDrawn;
    }

    /** Renders all objects visible by the provided view. */
    void RenderMan::RenderSingleView(const RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo)
    {
        const SceneInfo& sceneInfo = _scene->GetSceneInfo();
        auto& viewProps = view.GetProperties();

        SPtr<GpuParamBlockBuffer> perCameraBuffer = view.GetPerViewBuffer();
        perCameraBuffer->FlushToGPU();

        view.BeginFrame(frameInfo);

        RenderCompositorNodeInputs inputs(viewGroup, view, sceneInfo, *_options, frameInfo);

        const RenderCompositor& compositor = view.GetCompositor();
        compositor.Execute(inputs);

        view.EndFrame();
    }

    bool RenderMan::RenderOverlay(RendererView& view, const FrameInfo& frameInfo)
    {
        // view.GetPerViewBuffer()->FlushToGPU(); TODO
        view.BeginFrame(frameInfo);

        auto& viewProps = view.GetProperties();
        Camera* camera = view.GetSceneCamera();
        SPtr<RenderTarget> target = viewProps.Target.Target;
        SPtr<Viewport> viewport = camera->GetViewport();

        UINT32 clearFlags = viewport->GetClearFlags();

        RenderAPI& rapi = RenderAPI::Instance();
        if (clearFlags != 0)
        {
            rapi.SetRenderTarget(target);
            rapi.ClearViewport(clearFlags, viewport->GetClearColorValue(),
                viewport->GetClearDepthValue(), viewport->GetClearStencilValue());
        }
        else
        {
            rapi.SetRenderTarget(target, 0);
        }

        rapi.SetViewport(viewport->GetArea());

        view.EndFrame();

        return false;
    }

    void RenderMan::SetOptions(const SPtr<RendererOptions>& options)
    {
        _options = std::static_pointer_cast<RenderManOptions>(options);
        _scene->SetOptions(_options);
    }

    SPtr<RendererOptions> RenderMan::GetOptions() const
    {
        return _options;
    }

    void RenderMan::NotifyCameraAdded(Camera* camera)
    {
        // TE_PRINT("# Camera added");
        _scene->RegisterCamera(camera);
    }

    void RenderMan::NotifyCameraUpdated(Camera* camera, UINT32 updateFlag)
    {
        // TE_PRINT("# Camera updated");
        _scene->UpdateCamera(camera, updateFlag);
    }

    void RenderMan::NotifyCameraRemoved(Camera* camera)
    {
        // TE_PRINT("# Camera removed");
        _scene->UnregisterCamera(camera);
    }

    void RenderMan::NotifyRenderableAdded(Renderable* renderable)
    {
        // TE_PRINT("# Renderable added");
        _scene->RegisterRenderable(renderable);
    }

    void RenderMan::NotifyRenderableUpdated(Renderable* renderable)
    {
        // TE_PRINT("# Renderable updated");
        _scene->UpdateRenderable(renderable);
    }

    void RenderMan::NotifyRenderableRemoved(Renderable* renderable)
    {
        // TE_PRINT("# Renderable removed");
        _scene->UnregisterRenderable(renderable);
    }

    void RenderMan::NotifyLightAdded(Light* light)
    {
        // TE_PRINT("# Light added");
        _scene->RegisterLight(light);
    }

    void RenderMan::NotifyLightUpdated(Light* light)
    {
        // TE_PRINT("# Light updated");
        _scene->UpdateLight(light);
    }

    void RenderMan::NotifyLightRemoved(Light* light)
    {
        // TE_PRINT("# Light removed");
        _scene->UnregisterLight(light);
    }

    void RenderMan::NotifySkyboxAdded(Skybox* skybox)
    {
        // TE_PRINT("# Skybox added");
        _scene->RegisterSkybox(skybox);
    }

    void RenderMan::NotifySkyboxRemoved(Skybox* skybox)
    {
        // TE_PRINT("# Skybox removed");
        _scene->UnregisterSkybox(skybox);
    }

    SPtr<RenderMan> gRenderMan()
    {
        return std::static_pointer_cast<RenderMan>(RendererManager::Instance().GetRenderer());
    }
}
