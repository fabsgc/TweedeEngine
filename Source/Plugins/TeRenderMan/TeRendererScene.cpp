#include "TeRendererScene.h"
#include "Renderer/TeCamera.h"

namespace te
{
    RendererScene::RendererScene(const SPtr<RenderManOptions>& options)
        : _options(options)
    {
    }

    RendererScene::~RendererScene()
    {
    }

    void RendererScene::RegisterCamera(Camera* camera)
    {
        SPtr<RenderTarget> renderTarget = camera->GetViewport()->GetTarget();

        bool newCamera = true;
        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;

            if (camera == target.Camera)
            {
                newCamera = false;
                break;
            }
        }

        if (newCamera)
        {
            RendererRenderTarget rendererTarget;
            rendererTarget.Camera = camera;
            rendererTarget.Target = camera->GetViewport()->GetTarget();
            _info.RenderTargets.push_back(rendererTarget);
        }
    }

    void RendererScene::UpdateCamera(Camera* camera)
    {
        SPtr<RenderTarget> renderTarget = camera->GetViewport()->GetTarget();

        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;

            if (camera == target.Camera)
            {
                target.Target = renderTarget;
                break;
            }
        }
    }

    void RendererScene::UnregisterCamera(Camera* camera)
    {
        bool found = false;
        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;

            if (camera == target.Camera)
            {
                _info.RenderTargets.erase(iterTarget);
                break;
            }
        }
    }
}