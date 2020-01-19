#include "TeRenderMan.h"
#include "TeRendererScene.h"
#include "Manager/TeRendererManager.h"
#include "RenderAPI/TeRenderAPI.h"
#include "TeRenderManOptions.h"
#include "Renderer/TeCamera.h"

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

        for (auto& rtInfo : sceneInfo.RenderTargets)
        {
            if (rtInfo.target->GetProperties().IsWindow)
            {
                RenderOverlay(rtInfo.target, rtInfo.camera);
                RenderAPI::Instance().SwapBuffers(rtInfo.target);
            }
            else
            {
                RenderOverlay(rtInfo.target, rtInfo.camera);
                RenderAPI::Instance().SwapBuffers(rtInfo.target);
            }
        }
    }

    void RenderMan::RenderOverlay(const SPtr<RenderTarget> target, Camera* camera)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;

        if (clearBuffers != 0)
        {
            rapi.SetRenderTarget(target);
            rapi.ClearViewport(clearBuffers, camera->GetViewport()->GetClearColorValue());
        }
        else
        {
            rapi.SetRenderTarget(target);
        }
    }

    void RenderMan::NotifyCameraAdded(Camera* camera)
    {
        _scene->RegisterCamera(camera);
    }

    void RenderMan::NotifyCameraUpdated(Camera* camera)
    {
        _scene->UpdateCamera(camera);
    }

    void RenderMan::NotifyCameraRemoved(Camera* camera)
    {
        _scene->UnregisterCamera(camera);
    }

    SPtr<RenderMan> gRenderMan()
    {
        return std::static_pointer_cast<RenderMan>(RendererManager::Instance().GetRenderer());
    }
}
