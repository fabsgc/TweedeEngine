#include "TeRendererScene.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

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

            if (camera == target.camera)
            {
                newCamera = false;
                break;
            }
        }

        if (newCamera)
        {
            RendererRenderTarget rendererTarget;
            rendererTarget.camera = camera;
            rendererTarget.target = camera->GetViewport()->GetTarget();
            _info.RenderTargets.push_back(rendererTarget);
        }
    }

    void RendererScene::UpdateCamera(Camera* camera)
    {
        SPtr<RenderTarget> renderTarget = camera->GetViewport()->GetTarget();

        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;

            if (camera == target.camera)
            {
                target.target = renderTarget;
                break;
            }
        }
    }

    void RendererScene::UnregisterCamera(Camera* camera)
    {
        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;

            if (camera == target.camera)
            {
                _info.RenderTargets.erase(iterTarget);
                break;
            }
        }
    }
}
