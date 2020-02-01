#include "TeRenderMan.h"
#include "Manager/TeRendererManager.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Renderer/TeCamera.h"
#include "Utility/TeTime.h"

namespace te
{
    RenderMan::RenderMan()
    {}

    RenderMan::~RenderMan()
    {}

    void RenderMan::Initialize()
    {
        Renderer::Initialize();

        _options = te_shared_ptr_new<RenderManOptions>();
        _scene = te_shared_ptr_new<RendererScene>(_options);
    }

    void RenderMan::Destroy()
    {
        Renderer::Destroy();
        _scene = nullptr;
    }

    void RenderMan::Update()
    {}

    const String& RenderMan::GetName() const
    {
        static String name = "RenderMan";
        return name;
    }

    void RenderMan::RenderAll()
    {
        const SceneInfo& sceneInfo = _scene->GetSceneInfo();

        FrameTimings timings;
        timings.Time = gTime().GetTime();
        timings.TimeDelta = gTime().GetFrameDelta();
        timings.FrameIdx = gTime().GetFrameIdx();

        // Update global per-frame hardware buffers
        _scene->SetParamFrameParams(timings.Time);

        sceneInfo.RenderableReady.resize(sceneInfo.Renderables.size(), false);
        sceneInfo.RenderableReady.assign(sceneInfo.Renderables.size(), false);

        // Update per-frame data for all renderable objects
        for (UINT32 i = 0; i < sceneInfo.Renderables.size(); i++)
            _scene->PrepareRenderable(i);

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
        }
    }

    void RenderMan::RenderOverlay(const SPtr<RenderTarget> target, Camera* camera)
    {
        /*RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;

        if (clearBuffers != 0)
        {
            rapi.SetRenderTarget(target);
            rapi.ClearViewport(clearBuffers, camera->GetViewport()->GetClearColorValue());
        }
        else
        {
            rapi.SetRenderTarget(target);
        }*/
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
        _scene->RegisterCamera(camera);
    }

    void RenderMan::NotifyCameraUpdated(Camera* camera, UINT32 updateFlag)
    {
        _scene->UpdateCamera(camera, updateFlag);
    }

    void RenderMan::NotifyCameraRemoved(Camera* camera)
    {
        _scene->UnregisterCamera(camera);
    }

    void RenderMan::NotifyRenderableAdded(Renderable* renderable)
    {
        _scene->RegisterRenderable(renderable);
    }

    void RenderMan::NotifyRenderableRemoved(Renderable* renderable)
    {
        _scene->UnregisterRenderable(renderable);
    }

    void RenderMan::NotifyRenderableUpdated(Renderable* renderable)
    {
        _scene->UpdateRenderable(renderable);
    }

    void RenderMan::NotifyLightAdded(Light* light)
    {
        _scene->RegisterLight(light);
    }

    void RenderMan::NotifyLightUpdated(Light* light)
    {
        _scene->UpdateLight(light);
    }

    void RenderMan::NotifyLightRemoved(Light* light)
    {
        _scene->UnregisterLight(light);
    }

    SPtr<RenderMan> gRenderMan()
    {
        return std::static_pointer_cast<RenderMan>(RendererManager::Instance().GetRenderer());
    }
}
