#include "TeRendererScene.h"

#include "TeRenderMan.h"
#include "TeRenderManOptions.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeSkybox.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Material/TeTechnique.h"
#include "Material/TePass.h"
#include "RenderAPI/TeVertexData.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "Resources/TeBuiltinResources.h"
#include "Mesh/TeMesh.h"

namespace te
{
    PerFrameParamDef gPerFrameParamDef;

    /** Initializes a specific base pass technique on the provided material and returns the technique index. */
    static UINT32 InitAndRetrieveBasePassTechnique(Material& material)
    {
        UINT32 techniqueIdx  = material.GetDefaultTechnique();
        UINT32 numTechniques = material.GetNumTechniques();

        if (numTechniques == 0)
            TE_ASSERT_ERROR(false, "A material must a least have one technique");

        // Make sure the technique shaders are compiled
        const SPtr<Technique>& technique = material.GetTechnique(techniqueIdx);

        UINT32 numPasses = technique->GetNumPasses();
        if (numPasses == 0)
            TE_ASSERT_ERROR(false, "A technique must a least have one pass");

        if (technique)
            technique->Compile();

        return techniqueIdx;
    }

    static void ValidateBasePassMaterial(Material& material, UINT32 techniqueIdx, VertexDeclaration& vertexDecl)
    {
        // Validate mesh <-> shader vertex bindings
        UINT32 numPasses = material.GetNumPasses(techniqueIdx);
        for (UINT32 j = 0; j < numPasses; j++)
        {
            SPtr<Pass> pass = material.GetPass(j, techniqueIdx);
            SPtr<GraphicsPipelineState> graphicsPipeline = pass->GetGraphicsPipelineState();

            SPtr<VertexDeclaration> shaderDecl = graphicsPipeline->GetVertexProgram()->GetInputDeclaration();
            if (shaderDecl && !vertexDecl.IsCompatible(shaderDecl))
            {
                Vector<VertexElement> missingElements = vertexDecl.GetMissingElements(shaderDecl);

                if (!missingElements.empty())
                {
                    StringStream wrnStream;
                    wrnStream << "Provided mesh is missing required vertex attributes to render with the \
                        provided shader. Missing elements: " << std::endl;

                    for (auto& entry : missingElements)
                        wrnStream << "\t" << ToString(entry.GetSemantic()) << entry.GetSemanticIdx() << std::endl;

                    TE_DEBUG(wrnStream.str());
                    break;
                }
            }
        }
    }

    RendererScene::RendererScene(const SPtr<RenderManOptions>& options)
        : _options(options)
    { 
        _info.PerFrameParamBuffer = gPerFrameParamDef.CreateBuffer();
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
            view->NotifyNeedsRedraw();

        UINT32 updateEverythingFlag = (UINT32)ActorDirtyFlag::Everything
            | (UINT32)CameraDirtyFlag::Viewport;

        if ((updateFlag & updateEverythingFlag) != 0)
        {
            RENDERER_VIEW_DESC viewDesc = CreateViewDesc(camera);

            view->SetView(viewDesc);
            view->SetRenderSettings(camera->GetRenderSettings());

            UpdateCameraRenderTargets(camera);
        }

        if ((updateFlag & (UINT32)CameraDirtyFlag::RenderSettings) != 0)
            view->SetRenderSettings(camera->GetRenderSettings());

        const Transform& tfrm = camera->GetTransform();
        view->SetTransform(
            tfrm.GetPosition(),
            tfrm.GetForward(),
            camera->GetViewMatrix(),
            camera->GetProjectionMatrixRS(),
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
        else
        {
            if (light->GetType() == LightType::Radial)
            {
                UINT32 lightId = (UINT32)_info.RadialLights.size();
                light->SetRendererId(lightId);

                _info.RadialLights.push_back(RendererLight(light));
                _info.RadialLightWorldBounds.push_back(light->GetBounds());
            }
            else // Spot
            {
                UINT32 lightId = (UINT32)_info.SpotLights.size();
                light->SetRendererId(lightId);

                _info.SpotLights.push_back(RendererLight(light));
                _info.SpotLightWorldBounds.push_back(light->GetBounds());
            }
        }
    }

    /** Updates information about a previously registered light. */
    void RendererScene::UpdateLight(Light* light)
    {
        UINT32 lightId = light->GetRendererId();

        if (light->GetType() == LightType::Radial)
            _info.RadialLightWorldBounds[lightId] = light->GetBounds();
        else if (light->GetType() == LightType::Spot)
            _info.SpotLightWorldBounds[lightId] = light->GetBounds();
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
        else
        {
            if (light->GetType() == LightType::Radial)
            {
                Light* lastLight = _info.RadialLights.back()._internal;
                UINT32 lastLightId = lastLight->GetRendererId();

                if (lightId != lastLightId)
                {
                    // Swap current last element with the one we want to erase
                    std::swap(_info.RadialLights[lightId], _info.RadialLights[lastLightId]);
                    std::swap(_info.RadialLightWorldBounds[lightId], _info.RadialLightWorldBounds[lastLightId]);

                    lastLight->SetRendererId(lightId);
                }

                // Last element is the one we want to erase
                _info.RadialLights.erase(_info.RadialLights.end() - 1);
                _info.RadialLightWorldBounds.erase(_info.RadialLightWorldBounds.end() - 1);
            }
            else
            {
                Light* lastLight = _info.SpotLights.back()._internal;
                UINT32 lastLightId = lastLight->GetRendererId();

                if (lightId != lastLightId)
                {
                    // Swap current last element with the one we want to erase
                    std::swap(_info.SpotLights[lightId], _info.SpotLights[lastLightId]);
                    std::swap(_info.SpotLightWorldBounds[lightId], _info.SpotLightWorldBounds[lastLightId]);

                    lastLight->SetRendererId(lightId);
                }

                // Last element is the one we want to erase
                _info.SpotLights.erase(_info.SpotLights.end() - 1);
                _info.SpotLightWorldBounds.erase(_info.SpotLightWorldBounds.end() - 1);
            }
        }
    }

    /** Registers a new renderable object in the scene. */
    void RendererScene::RegisterRenderable(Renderable* renderable)
    { 
        UINT32 renderableId = (UINT32)_info.Renderables.size();

        renderable->SetRendererId(renderableId);
        _info.Renderables.push_back(te_new<RendererRenderable>());
        _info.RenderableCullInfos.push_back(CullInfo(renderable->GetBounds(), renderable->GetLayer(), renderable->GetCullDistanceFactor()));

        RendererRenderable* rendererRenderable = _info.Renderables.back();
        rendererRenderable->RenderablePtr = renderable;
        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PrevWorldTfrm = rendererRenderable->WorldTfrm;
        rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Clean;
        rendererRenderable->UpdatePerObjectBuffer();

        SetMeshData(rendererRenderable, renderable);

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (renderable->GetInstancing() && iter == _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.push_back(rendererRenderable);
            else if (!renderable->GetInstancing() && iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }
    }

    /** Updates information about a previously registered renderable object. */
    void RendererScene::UpdateRenderable(Renderable* renderable)
    {
        UINT32 renderableId = renderable->GetRendererId();

        // If element can be merged, we check if it still exist on renderer side
        if (renderable->GetCanBeMerged())
        {
            if (renderableId > _info.Renderables.size())
                return;
            if (_info.Renderables[renderableId]->RenderablePtr != renderable)
                return;
        }

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];

        if(rendererRenderable->PreviousFrameDirtyState != PrevFrameDirtyState::Updated)
            rendererRenderable->PrevWorldTfrm = rendererRenderable->WorldTfrm;

        rendererRenderable->WorldTfrm = renderable->GetMatrix();
        rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Updated;

        _info.Renderables[renderableId]->UpdatePerObjectBuffer();
        _info.RenderableCullInfos[renderableId].Layer = renderable->GetLayer();
        _info.RenderableCullInfos[renderableId].Boundaries = renderable->GetBounds();
        _info.RenderableCullInfos[renderableId].CullDistanceFactor = renderable->GetCullDistanceFactor();

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (renderable->GetInstancing() && iter == _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.push_back(rendererRenderable);
            else if (!renderable->GetInstancing() && iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }

        UINT32 dirtyFlag = renderable->GetCoreDirtyFlags();
        if (dirtyFlag & (UINT32)ActorDirtyFlag::GpuParams)
            SetMeshData(rendererRenderable, renderable);
    }

    /** Removes a renderable object from the scene. */
    void RendererScene::UnregisterRenderable(Renderable* renderable)
    { 
        UINT32 renderableId = renderable->GetRendererId();

        // If element can be merged, we check if it still exist on renderer side
        if (renderable->GetCanBeMerged())
        {
            if (renderableId > _info.Renderables.size())
                return;
            if (_info.Renderables[renderableId]->RenderablePtr != renderable)
                return;
        }

        Renderable* lastRenderable = _info.Renderables.back()->RenderablePtr;
        UINT32 lastRenderableId = lastRenderable->GetRendererId();

        RendererRenderable* rendererRenderable = _info.Renderables[renderableId];
        
        if (renderableId != lastRenderableId)
        {
            // Swap current last element with the one we want to erase
            std::swap(_info.Renderables[renderableId], _info.Renderables[lastRenderableId]);
            std::swap(_info.RenderableCullInfos[renderableId], _info.RenderableCullInfos[lastRenderableId]);

            lastRenderable->SetRendererId(renderableId);
        }

        if (_options->InstancingMode == RenderManInstancing::Manual)
        {
            auto iter = std::find(_info.RenderablesInstanced.begin(), _info.RenderablesInstanced.end(), rendererRenderable);
            if (iter != _info.RenderablesInstanced.end())
                _info.RenderablesInstanced.erase(iter);
        }

        // Last element is the one we want to erase
        _info.Renderables.erase(_info.Renderables.end() - 1);
        _info.RenderableCullInfos.erase(_info.RenderableCullInfos.end() - 1);

        te_delete(rendererRenderable);
    }

    void RendererScene::BatchRenderables()
    { }

    void RendererScene::SetMeshData(RendererRenderable* rendererRenderable, Renderable* renderable)
    {
        SPtr<Mesh> mesh = renderable->GetMesh();
        if (mesh != nullptr)
        {
            MeshProperties& meshProps = mesh->GetProperties();
            SPtr<VertexDeclaration> vertexDecl = mesh->GetVertexData()->vertexDeclaration;

            for (UINT32 i = 0; i < meshProps.GetNumSubMeshes(); i++)
            {
                RenderableElement* renElement = nullptr;

                if (rendererRenderable->Elements.size() == i)
                    rendererRenderable->Elements.push_back(RenderableElement());

                renElement = &rendererRenderable->Elements[i];

                renElement->Type = (UINT32)RenderElementType::Renderable;
                renElement->MeshElem = mesh;
                renElement->SubMeshElem = meshProps.GetSubMeshPtr(i);
                renElement->BoneMatrixBuffer = renderable->GetBoneMatrixBuffer();
                renElement->BonePrevMatrixBuffer = renderable->GetBonePrevMatrixBuffer();
                renElement->AnimType = renderable->GetAnimType();
                renElement->AnimationId = renderable->GetAnimationId();

                renElement->MaterialElem = renderable->GetMaterial(i);
                if (renElement->MaterialElem == nullptr)
                    renElement->MaterialElem = nullptr;

                if (renElement->MaterialElem != nullptr && renElement->MaterialElem->GetShader() == nullptr)
                    renElement->MaterialElem = nullptr;

                // If no material use the default material
                if (renElement->MaterialElem == nullptr)
                    renElement->MaterialElem = gBuiltinResources().GetDefaultMaterial().GetInternalPtr();

                // Determine which technique to use
                renElement->DefaultTechniqueIdx = InitAndRetrieveBasePassTechnique(*renElement->MaterialElem);

                // Generate or assigned renderer specific data for the material
                renElement->MaterialElem->CreateGpuParams(renElement->DefaultTechniqueIdx, renElement->GpuParamsElem);

                // We update gpu paremeters such as diffuse or specular defined for this material
                PerObjectBuffer::UpdatePerMaterial(renElement->PerMaterialParamBuffer, renElement->MaterialElem->GetProperties());

                // Set renderable properties to renderElement
                renElement->Properties = &renderable->GetProperties();

#if TE_DEBUG_MODE
                ValidateBasePassMaterial(*renElement->MaterialElem, renElement->DefaultTechniqueIdx, *vertexDecl);
#endif
            }

            // Prepare all parameter bindings
            for (auto& element : rendererRenderable->Elements)
            {
                SPtr<Shader> shader = element.MaterialElem->GetShader();
                if (shader == nullptr)
                {
                    TE_DEBUG("Missing shader on material.");
                    continue;
                }

                for (auto& gpuParams : element.GpuParamsElem)
                {
                    gpuParams->SetParamBlockBuffer("PerObjectBuffer", rendererRenderable->PerObjectParamBuffer);
                    gpuParams->SetParamBlockBuffer("PerMaterialBuffer", element.PerMaterialParamBuffer);

                    if (gpuParams->HasBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices"))
                        gpuParams->SetBuffer(GPT_VERTEX_PROGRAM, "BoneMatrices", element.BoneMatrixBuffer);

                    if (gpuParams->HasBuffer(GPT_VERTEX_PROGRAM, "PrevBoneMatrices"))
                        gpuParams->SetBuffer(GPT_VERTEX_PROGRAM, "PrevBoneMatrices", element.BonePrevMatrixBuffer);
                }
            }
        }
    }

    void RendererScene::RegisterSkybox(Skybox* skybox)
    {
        _info.SkyboxElem = skybox;
    }

    void RendererScene::UnregisterSkybox(Skybox* skybox)
    {
        if (_info.SkyboxElem == skybox)
            _info.SkyboxElem = nullptr;
    }

    void RendererScene::SetOptions(const SPtr<RenderManOptions>& options)
    {
        _options = options;

        for (auto& entry : _info.Views)
            entry->SetStateReductionMode(_options->ReductionMode);
    }

    void RendererScene::SetParamFrameParams(const float& time, const float& delta)
    {
        gPerFrameParamDef.gTime.Set(_info.PerFrameParamBuffer, time);
        gPerFrameParamDef.gFrameDelta.Set(_info.PerFrameParamBuffer, delta);
    }

    void RendererScene::SetParamCameraParams(const Color& sceneLightColor)
    {
        gPerFrameParamDef.gSceneLightColor.Set(_info.PerFrameParamBuffer, sceneLightColor.GetAsVector4());
    }

    void RendererScene::SetParamSkyboxParams(bool enabled)
    {
        if(_info.SkyboxElem != nullptr && enabled)
        {
            gPerFrameParamDef.gSkyboxBrightness.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetBrightness());
            gPerFrameParamDef.gUseSkyboxMap.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetTexture() ? 1 : 0);
            gPerFrameParamDef.gUseSkyboxIrradianceMap.Set(_info.PerFrameParamBuffer, _info.SkyboxElem->GetIrradiance() ? 1 : 0);
        }
        else
        {
            gPerFrameParamDef.gSkyboxBrightness.Set(_info.PerFrameParamBuffer, 1.0f);
            gPerFrameParamDef.gUseSkyboxMap.Set(_info.PerFrameParamBuffer, 0);
            gPerFrameParamDef.gUseSkyboxIrradianceMap.Set(_info.PerFrameParamBuffer, 0);
        }
    }

    void RendererScene::PrepareRenderable(UINT32 idx, const FrameInfo& frameInfo)
    {
        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        if (rendererRenderable->PreviousFrameDirtyState != PrevFrameDirtyState::Clean)
        {
            if (rendererRenderable->PreviousFrameDirtyState == PrevFrameDirtyState::Updated)
                rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::CopyMostRecent;
            else if (rendererRenderable->PreviousFrameDirtyState == PrevFrameDirtyState::CopyMostRecent)
            {
                rendererRenderable->PrevWorldTfrm = _info.Renderables[idx]->WorldTfrm;
                rendererRenderable->PreviousFrameDirtyState = PrevFrameDirtyState::Clean;
                rendererRenderable->UpdatePerObjectBuffer();
            }
        }
    }

    void RendererScene::PrepareVisibleRenderable(UINT32 idx, const FrameInfo& frameInfo)
    {
        if (_info.RenderableReady[idx])
            return;

        RendererRenderable* rendererRenderable = _info.Renderables[idx];

        if(frameInfo.PerFrameDatas.Animation != nullptr)
            rendererRenderable->RenderablePtr->UpdateAnimationBuffers(*frameInfo.PerFrameDatas.Animation);

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

        UINT32 flag = (UINT32)camera->GetFlags();
        viewDesc.RunPostProcessing = true;
        viewDesc.OnDemand = (flag & (UINT32)CameraFlag::OnDemand) ? true : false;

        viewDesc.CullFrustum = camera->GetWorldFrustum();
        viewDesc.VisibleLayers = camera->GetLayers();
        viewDesc.NearPlane = camera->GetNearClipDistance();
        viewDesc.FarPlane = camera->GetFarClipDistance();
        viewDesc.FlipView = false;

        const Transform& tfrm = camera->GetTransform();
        viewDesc.ViewOrigin = tfrm.GetPosition();
        viewDesc.ViewDirection = tfrm.GetForward();
        viewDesc.ProjTransform = camera->GetProjectionMatrixRS();
        viewDesc.ViewTransform = camera->GetViewMatrix();
        viewDesc.ProjType = camera->GetProjectionType();

        viewDesc.ReductionMode = _options->ReductionMode;
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
