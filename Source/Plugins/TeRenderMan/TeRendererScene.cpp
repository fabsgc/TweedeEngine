#include "TeRendererScene.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

namespace te
{
    PerFrameParamDef gPerFrameParamDef;

    RendererScene::RendererScene(const SPtr<RenderManOptions>& options)
        : _options(options)
    { 
        _perFrameParamBuffer = gPerFrameParamDef.CreateBuffer();
    }

    RendererScene::~RendererScene()
    { 
        for (auto& entry : _info.Renderables)
            te_delete(entry);

        for (auto& entry : _info.Views)
            te_delete(entry);
    }

    void RendererScene::RegisterCamera(Camera* camera)
    {
        RENDERER_VIEW_DESC viewDesc = CreateViewDesc(camera);

        RendererView* view = te_new<RendererView>(viewDesc);
        view->SetRenderSettings(camera->GetRenderSettings());
        view->UpdatePerViewBuffer();

        UINT32 viewIdx = (UINT32)_info.Views.size();
        _info.Views.push_back(view);

        _info.CameraToView[camera] = viewIdx;
        camera->SetRendererId(viewIdx);

        UpdateCameraRenderTargets(camera);
    }

    void RendererScene::UpdateCamera(Camera* camera, UINT32 updateFlag)
    {
        UINT32 cameraId = camera->GetRendererId();
        RendererView* view = _info.Views[cameraId];

        if ((updateFlag & (UINT32)CameraDirtyFlag::Redraw) != 0)
            view->_notifyNeedsRedraw();

        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything
            | (UINT32)CameraDirtyFlag::Viewport;

        if ((updateFlag & updateEverythingFlag) != 0)
        {
            RENDERER_VIEW_DESC viewDesc = CreateViewDesc(camera);

            view->SetView(viewDesc);
            view->SetRenderSettings(camera->GetRenderSettings());

            UpdateCameraRenderTargets(camera);
            return;
        }

        if ((updateFlag & (UINT32)CameraDirtyFlag::RenderSettings) != 0)
            view->SetRenderSettings(camera->GetRenderSettings());

        const Transform& tfrm = camera->GetTransform();
        view->SetTransform(
            tfrm.GetPosition(),
            tfrm.GetForward(),
            camera->GetViewMatrix(),
            camera->GetProjectionMatrix(),
            camera->GetWorldFrustum());

        view->UpdatePerViewBuffer();
    }

    void RendererScene::UnregisterCamera(Camera* camera)
    {
        UINT32 cameraId = camera->GetRendererId();

        Camera* lastCamera = _info.Views.back()->GetSceneCamera();
        UINT32 lastCameraId = lastCamera->GetRendererId();

        if (cameraId != lastCameraId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Views[cameraId], _info.Views[lastCameraId]);
            lastCamera->SetRendererId(cameraId);

            _info.CameraToView[lastCamera] = cameraId;
        }

        // Last element is the one we want to erase
        RendererView* view = _info.Views[_info.Views.size() - 1];
        te_delete(view);

        _info.Views.erase(_info.Views.end() - 1);

        auto iterFind = _info.CameraToView.find(camera);
        if (iterFind != _info.CameraToView.end())
            _info.CameraToView.erase(iterFind);

        UpdateCameraRenderTargets(camera, true);
    }

    /** Registers a new light in the scene. */
    void RendererScene::RegisterLight(Light* light)
    { 
        if (light->GetType() == LightType::Directional)
        {
            UINT32 lightId = (UINT32)_info.DirectionalLights.size();
            light->SetRendererId(lightId);

            _info.DirectionalLights.push_back(RendererLight(light));
        }
    }

    /** Updates information about a previously registered light. */
    void RendererScene::UpdateLight(Light* light)
    {
        // Nothing for the moment
    }

    /** Removes a light from the scene. */
    void RendererScene::UnregisterLight(Light* light)
    {
        UINT32 lightId = light->GetRendererId();
        if (light->GetType() == LightType::Directional)
        {
            Light* lastLight = _info.DirectionalLights.back()._internal;
            UINT32 lastLightId = lastLight->GetRendererId();

            if (lightId != lastLightId)
            {
                // Swap current last element with the one we want to erase
                std::swap(_info.DirectionalLights[lightId], _info.DirectionalLights[lastLightId]);
                lastLight->SetRendererId(lightId);
            }

            // Last element is the one we want to erase
            _info.DirectionalLights.erase(_info.DirectionalLights.end() - 1);
        }
    }

    /** Registers a new renderable object in the scene. */
    void RendererScene::RegisterRenderable(Renderable* renderable)
    { 
        UINT32 renderableId = (UINT32)_info.Renderables.size();

        renderable->SetRendererId(renderableId);
        _info.Renderables.push_back(te_new<RendererRenderable>());
        _info.RenderableCullInfos.push_back(CullInfo(renderable->GetBounds(), renderable->GetCullDistanceFactor()));

        RendererRenderable* rendererRenderable = _info.Renderables.back();
        rendererRenderable->RenderablePtr = renderable;
        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PrevFrameDirtyState = PrevFrameDirtyState::Clean;
        rendererRenderable->UpdatePerObjectBuffer();

        SPtr<Mesh> mesh = renderable->GetMesh();
        if (mesh != nullptr)
        {
            const MeshProperties& meshProps = mesh->GetProperties();
            SPtr<VertexDeclaration> vertexDecl = mesh->GetVertexData()->vertexDeclaration;

            for (UINT32 i = 0; i < meshProps.GetNumSubMeshes(); i++)
            {
                rendererRenderable->Elements.push_back(RenderableElement());
                RenderableElement& renElement = rendererRenderable->Elements.back();

                renElement.Type = (UINT32)RenderElementType::Renderable;
                renElement.MeshElem = mesh;
            }
        }

        // Prepare all parameter bindings
        for (auto& element : rendererRenderable->Elements)
        {
            // TODO
        }
    }

    /** Updates information about a previously registered renderable object. */
    void RendererScene::UpdateRenderable(Renderable* renderable)
    {
        UINT32 renderableId = renderable->GetRendererId();

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];

        if(rendererRenderable->PrevFrameDirtyState != PrevFrameDirtyState::Updated)
            rendererRenderable->PrevWorldTfrm = rendererRenderable->WorldTfrm;

        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PrevFrameDirtyState = PrevFrameDirtyState::Updated;

        _info.Renderables[renderableId]->UpdatePerObjectBuffer();
        _info.RenderableCullInfos[renderableId].Boundaries = renderable->GetBounds();
        _info.RenderableCullInfos[renderableId].CullDistanceFactor = renderable->GetCullDistanceFactor();
    }

    /** Removes a renderable object from the scene. */
    void RendererScene::UnregisterRenderable(Renderable* renderable)
    { 
        UINT32 renderableId = renderable->GetRendererId();
        Renderable* lastRenderable = _info.Renderables.back()->RenderablePtr;
        UINT32 lastRenderableId = lastRenderable->GetRendererId();

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];
        Vector<RenderableElement>& elements = rendererRenderable->Elements;
        
        if (renderableId != lastRenderableId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Renderables[renderableId], _info.Renderables[lastRenderableId]);
            std::swap(_info.RenderableCullInfos[renderableId], _info.RenderableCullInfos[lastRenderableId]);

            lastRenderable->SetRendererId(renderableId);
        }

        // Last element is the one we want to erase
        _info.Renderables.erase(_info.Renderables.end() - 1);
        _info.RenderableCullInfos.erase(_info.RenderableCullInfos.end() - 1);

        te_delete(rendererRenderable);
    }

    void RendererScene::SetOptions(const SPtr<RenderManOptions>& options)
    {
        _options = options;
    }

    void RendererScene::SetParamFrameParams(float time)
    {
        gPerFrameParamDef.gTime.Set(_perFrameParamBuffer, time);
    }

    void RendererScene::PrepareRenderable(UINT32 idx)
    {
        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        if (rendererRenderable->PrevFrameDirtyState != PrevFrameDirtyState::Clean)
        {
            if (rendererRenderable->PrevFrameDirtyState == PrevFrameDirtyState::Updated)
                rendererRenderable->PrevFrameDirtyState = PrevFrameDirtyState::CopyMostRecent;
            else if (rendererRenderable->PrevFrameDirtyState == PrevFrameDirtyState::CopyMostRecent)
            {
                rendererRenderable->PrevWorldTfrm = _info.Renderables[idx]->WorldTfrm;
                rendererRenderable->PrevFrameDirtyState = PrevFrameDirtyState::Clean;
                rendererRenderable->UpdatePerObjectBuffer();
            }
        }
    }

    void RendererScene::PrepareVisibleRenderable(UINT32 idx)
    {
        if (_info.RenderableReady[idx])
            return;

        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        _info.Renderables[idx]->PerObjectParamBuffer->FlushToGPU();
        _info.RenderableReady[idx] = true;
    }

    RENDERER_VIEW_DESC RendererScene::CreateViewDesc(Camera* camera) const
    {
        SPtr<Viewport> viewport = camera->GetViewport();
        RENDERER_VIEW_DESC viewDesc;

        viewDesc.Target.ClearFlags = viewport->GetClearFlags();

        viewDesc.Target.ClearColor = viewport->GetClearColorValue();
        viewDesc.Target.ClearDepthValue = viewport->GetClearDepthValue();
        viewDesc.Target.ClearStencilValue = viewport->GetClearStencilValue();

        viewDesc.Target.Target = viewport->GetTarget();
        viewDesc.Target.NrmViewRect = viewport->GetArea();
        viewDesc.Target.ViewRect = viewport->GetPixelArea();

        if (viewDesc.Target.Target != nullptr)
        {
            viewDesc.Target.TargetWidth = viewDesc.Target.Target->GetProperties().Width;
            viewDesc.Target.TargetHeight = viewDesc.Target.Target->GetProperties().Height;
        }
        else
        {
            viewDesc.Target.TargetWidth = 0;
            viewDesc.Target.TargetHeight = 0;
        }

        viewDesc.Target.NumSamples = camera->GetMSAACount();

        viewDesc.MainView = camera->IsMain();
        viewDesc.OnDemand = (camera->GetFlags() | (UINT32)CameraFlag::OnDemand) ? true : false;

        viewDesc.CullFrustum = camera->GetWorldFrustum();
        viewDesc.NearPlane = camera->GetNearClipDistance();
        viewDesc.FarPlane = camera->GetFarClipDistance();
        viewDesc.FlipView = false;

        const Transform& tfrm = camera->GetTransform();
        viewDesc.ViewOrigin = tfrm.GetPosition();
        viewDesc.ViewDirection = tfrm.GetForward();
        viewDesc.ProjTransform = camera->GetProjectionMatrixRS();
        viewDesc.ViewTransform = camera->GetViewMatrix();
        viewDesc.ProjType = camera->GetProjectionType();

        viewDesc.SceneCamera = camera;

        return viewDesc;
    }

    void RendererScene::UpdateCameraRenderTargets(Camera* camera, bool remove)
    {
        SPtr<RenderTarget> renderTarget = camera->GetViewport()->GetTarget();

        // Remove from render target list
        int rtChanged = 0; // 0 - No RT, 1 - RT found, 2 - RT changed
        for (auto iterTarget = _info.RenderTargets.begin(); iterTarget != _info.RenderTargets.end(); ++iterTarget)
        {
            RendererRenderTarget& target = *iterTarget;
            for (auto iterCam = target.Cameras.begin(); iterCam != target.Cameras.end(); ++iterCam)
            {
                if (camera == *iterCam)
                {
                    if (remove)
                    {
                        target.Cameras.erase(iterCam);
                        rtChanged = 1;
                    }
                    else
                    {
                        if (renderTarget != target.Target)
                        {
                            target.Cameras.erase(iterCam);
                            rtChanged = 2;
                        }
                        else
                            rtChanged = 1;
                    }

                    break;
                }
            }

            if (target.Cameras.empty())
            {
                _info.RenderTargets.erase(iterTarget);
                break;
            }
        }

        // Register in render target list
        if (renderTarget != nullptr && !remove && (rtChanged == 0 || rtChanged == 2))
        {
            auto findIter = std::find_if(_info.RenderTargets.begin(), _info.RenderTargets.end(),
                [&](const RendererRenderTarget& x) { return x.Target == renderTarget; });

            if (findIter != _info.RenderTargets.end())
            {
                findIter->Cameras.push_back(camera);
            }
            else
            {
                _info.RenderTargets.push_back(RendererRenderTarget());
                RendererRenderTarget& renderTargetData = _info.RenderTargets.back();

                renderTargetData.Target = renderTarget;
                renderTargetData.Cameras.push_back(camera);
            }

            // Sort render targets based on priority
            auto cameraComparer = [&](const Camera* a, const Camera* b) { return a->GetPriority() > b->GetPriority(); };
            auto renderTargetInfoComparer = [&](const RendererRenderTarget& a, const RendererRenderTarget& b)
            { return a.Target->GetProperties().Priority > b.Target->GetProperties().Priority; };
            std::sort(begin(_info.RenderTargets), end(_info.RenderTargets), renderTargetInfoComparer);

            for (auto& camerasPerTarget : _info.RenderTargets)
            {
                Vector<Camera*>& cameras = camerasPerTarget.Cameras;
                std::sort(begin(cameras), end(cameras), cameraComparer);
            }
        }
    }
}
