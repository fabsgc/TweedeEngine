#include "TeRenderMan.h"
#include "TeRendererView.h"
#include "TeRendererScene.h"
#include "TeRendererTextures.h"
#include "TeRenderManOptions.h"
#include "TeRenderManIBLUtility.h"
#include "TeRenderCompositor.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeRendererUtility.h"
#include "Renderer/TeGpuResourcePool.h"
#include "Renderer/TeIBLUtility.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Manager/TeRendererManager.h"
#include "CoreUtility/TeCoreObjectManager.h"
#include "Profiling/TeProfilerGPU.h"
#include "Utility/TeTime.h"
#include "Gui/TeGuiAPI.h"
#include "Mesh/TeMesh.h"

namespace te
{
    SPtr<GpuParamBlockBuffer> gPerInstanceParamBuffer[STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER] = { nullptr };

    RenderMan::RenderMan()
        : _renderAPI(RenderAPI::Instance())
    { }

    RenderMan::~RenderMan()
    { }

    void RenderMan::Initialize()
    {
        Renderer::Initialize();

        if(!RendererUtility::IsStarted())
            RendererUtility::StartUp();
        if(!GpuResourcePool::IsStarted())
            GpuResourcePool::StartUp();
        if(!IBLUtility::IsStarted())
            IBLUtility::StartUp<RenderManIBLUtility>();

        RendererTextures::StartUp();

        _options = te_shared_ptr_new<RenderManOptions>();
        _options->InstancingMode = RenderManInstancing::Manual;

        _scene = te_shared_ptr_new<RendererScene>(_options);

        _mainViewGroup = te_new<RendererViewGroup>(nullptr, 0, _options);

        RenderCompositor::RegisterNodeType<RCNodeGpuInitializationPass>();
        RenderCompositor::RegisterNodeType<RCNodeZPrePass>();
        RenderCompositor::RegisterNodeType<RCNodeForwardPass>();
        RenderCompositor::RegisterNodeType<RCNodeSkybox>();
        RenderCompositor::RegisterNodeType<RCNodeForwardTransparentPass>();
        RenderCompositor::RegisterNodeType<RCNodeTonemapping>();
        RenderCompositor::RegisterNodeType<RCNodeMotionBlur>();
        RenderCompositor::RegisterNodeType<RCNodeGaussianDOF>();
        RenderCompositor::RegisterNodeType<RCNodeFXAA>();
        RenderCompositor::RegisterNodeType<RCNodeTemporalAA>();
        RenderCompositor::RegisterNodeType<RCNodeSSAO>();
        RenderCompositor::RegisterNodeType<RCNodeBloom>();
        RenderCompositor::RegisterNodeType<RCNodeResolvedSceneDepth>();
        RenderCompositor::RegisterNodeType<RCNodeHalfSceneTex>();
        RenderCompositor::RegisterNodeType<RCNodeSceneTexDownsamples>();
        RenderCompositor::RegisterNodeType<RCNodePostProcess>();
        RenderCompositor::RegisterNodeType<RCNodeFinalResolve>();
    }

    void RenderMan::Destroy()
    {
        for (UINT32 i = 0; i < STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER; i++)
        {
            if (gPerInstanceParamBuffer[i])
            {
                gPerInstanceParamBuffer[i]->Destroy();
                gPerInstanceParamBuffer[i] = nullptr;
            }
        }

        if (gPerLightsParamBuffer)
        {
            gPerLightsParamBuffer->Destroy();
            gPerLightsParamBuffer = nullptr;
        }

        _scene = nullptr;

        RenderCompositor::CleanUp();

        te_delete(_mainViewGroup);

        RendererTextures::ShutDown();

        if(GpuResourcePool::IsStarted())
            GpuResourcePool::ShutDown();
        if(RendererUtility::IsStarted())
            RendererUtility::ShutDown();
        if(RenderManIBLUtility::IsStarted())
            IBLUtility::ShutDown();

        Renderer::Destroy();
    }

    void RenderMan::Update()
    { }

    const String& RenderMan::GetName() const
    {
        static String name = "RenderMan";
        return name;
    }

    void RenderMan::RenderAll(FrameData& frameData)
    {
        gProfilerGPU().BeginFrame();

        _renderTextures.Clear();

        CoreObjectManager::Instance().FrameSync();

        const SceneInfo& sceneInfo = _scene->GetSceneInfo();

        FrameTimings timings;
        timings.Time = gTime().GetTime();
        timings.TimeDelta = gTime().GetFrameDelta();
        timings.FrameIdx = gTime().GetFrameIdx();

        // Update global per-frame hardware buffers
        _scene->SetParamFrameParams(timings.Time, timings.TimeDelta);

        sceneInfo.RenderableReady.resize(sceneInfo.Renderables.size(), false);
        sceneInfo.RenderableReady.assign(sceneInfo.Renderables.size(), false);

        FrameInfo frameInfo(timings, frameData);

        // Update per-frame data for all renderable objects
        for (UINT32 i = 0; i < sceneInfo.Renderables.size(); i++)
            _scene->PrepareRenderable(i, frameInfo);

        // Gather all views
        for (auto& rtInfo : sceneInfo.RenderTargets)
        {
            _renderAPI.PushMarker("[DRAW] RenderTarget", Color(0.8f, 0.4f, 0.4f));

            bool anythingDrawn = false;
            Vector<RendererView*> views;
            SPtr<RenderTarget> target = rtInfo.Target;
            const Vector<Camera*>& cameras = rtInfo.Cameras;

            UINT32 numCameras = (UINT32)cameras.size();
            for (UINT32 i = 0; i < numCameras; i++)
            {
                //If we have a camera without any render target, don't process it at all
                if (!cameras[i]->GetViewport()->GetTarget())
                    continue;

                UINT32 viewIdx = sceneInfo.CameraToView.at(cameras[i]);
                RendererView* viewInfo = sceneInfo.Views[viewIdx];
                views.push_back(viewInfo);
            }

            _mainViewGroup->SetViews(views.data(), (UINT32)views.size());

            if (_options->CullingFlags & (UINT32)RenderManCulling::Frustum ||
                _options->CullingFlags & (UINT32)RenderManCulling::Occlusion)
            {
                _mainViewGroup->DetermineVisibility(sceneInfo);
            }
            else // Set all objects as visible
            {
                _mainViewGroup->SetAllObjectsAsVisible(sceneInfo);
            }

            for (auto& view : views)
            {
                _renderAPI.PushMarker("[DRAW] View",Color(0.4f, 0.8f, 0.4f));

                _mainViewGroup->GenerateInstanced(sceneInfo, _options->InstancingMode);
                _mainViewGroup->GenerateRenderQueue(sceneInfo, *view, _options->InstancingMode);

                const auto& settings = view->GetSceneCamera()->GetRenderSettings();

                _scene->SetParamCameraParams(settings->SceneLightColor);
                _scene->SetParamSkyboxParams(view->GetSceneCamera()->GetRenderSettings()->EnableSkybox);
                _scene->SetParamHDRParams(settings->EnableHDR, settings->Tonemapping.Enabled, settings->Gamma, 
                    settings->ExposureScale, settings->Contrast, settings->Brightness);

                if (RenderSingleView(*_mainViewGroup, *view, frameInfo))
                    anythingDrawn = true;

                _renderAPI.PopMarker();
            }

            if (rtInfo.Target->GetProperties().IsWindow && anythingDrawn)
            {
                _renderAPI.SwapBuffers(rtInfo.Target);
            }

            _renderAPI.PopMarker();
        }

        GpuResourcePool::Instance().Update();

        gProfilerGPU().EndFrame();
    }

    /** Renders all views in the provided view group. Returns true if anything has been draw to any of the views. */
    bool RenderMan::RenderSingleView(RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo)
    {
        bool needs3DRender = false;
        bool anythingDrawn = false;

        if (!view.ShouldDraw())
            return anythingDrawn;
        
        if (view.ShouldDraw3D())
            needs3DRender = true;

        if (needs3DRender)
        {
            const SceneInfo& sceneInfo = _scene->GetSceneInfo();
            const VisibilityInfo& visibility = viewGroup.GetVisibilityInfo();

            Vector3I lightCounts;
            const PerLightData* lights[STANDARD_FORWARD_MAX_NUM_LIGHTS];

            // Find influencing lights for this view
            // Update Light Buffers
            {
                for (const auto& element : view.GetOpaqueQueue()->GetSortedElements())
                {
                    if (!element.RenderElem->Properties->CastLights)
                        continue;
                    if ((lightCounts.x + lightCounts.y + lightCounts.z) == STANDARD_FORWARD_MAX_NUM_LIGHTS)
                        break;

                    // Compute list of lights that influence this Mesh
                    const Bounds& bounds = element.RenderElem->MeshElem->GetProperties().GetBounds();
                    viewGroup.GetVisibleLightData().GatherInfluencingLights(bounds, lights, lightCounts);
                }
                PerLightsBuffer::UpdatePerLights(lights, lightCounts.x + lightCounts.y + lightCounts.z);
            }

            // Render shadow maps
            // Render only shadow maps for lights needed for this view
            // If a shadow map has been drawn by a previous view, do not draw it again
            {
                // TODO
            }

            // Update various buffers required by each renderable
            {
                UINT32 numRenderables = (UINT32)sceneInfo.Renderables.size();
                for (UINT32 i = 0; i < numRenderables; i++)
                {
                    if (!visibility.Renderables[i].Visible && !visibility.Renderables[i].Instanced)
                        continue;

                    _scene->PrepareVisibleRenderable(i, frameInfo);
                }
            }
        }

        const RenderSettings& settings = view.GetRenderSettings();
        if (settings.OverlayOnly)
        {
            if (RenderOverlay(view, frameInfo))
                anythingDrawn = true;
        }
        else if (needs3DRender)
        {
            RenderSingleViewInternal(viewGroup, view, frameInfo);
            anythingDrawn = true;
        }

        return anythingDrawn;
    }

    /** Renders all objects visible by the provided view. */
    void RenderMan::RenderSingleViewInternal(const RendererViewGroup& viewGroup, RendererView& view, const FrameInfo& frameInfo)
    {
        const SceneInfo& sceneInfo = _scene->GetSceneInfo();

        SPtr<GpuParamBlockBuffer> perCameraBuffer = view.GetPerViewBuffer();

        view.BeginFrame(frameInfo);

        RenderCompositorNodeInputs inputs(viewGroup, view, sceneInfo, *_options, frameInfo, *this, _renderAPI);

        const RenderCompositor& compositor = view.GetCompositor();
        compositor.Execute(inputs);

        view.EndFrame();
    }

    bool RenderMan::RenderOverlay(RendererView& view, const FrameInfo& frameInfo)
    {
        view.GetPerViewBuffer()->FlushToGPU();
        view.BeginFrame(frameInfo);

        auto& viewProps = view.GetProperties();
        Camera* camera = view.GetSceneCamera();
        SPtr<RenderTarget> target = viewProps.Target.Target;
        SPtr<Viewport> viewport = camera->GetViewport();

        UINT32 clearFlags = viewport->GetClearFlags();

        if (clearFlags != 0)
        {
            _renderAPI.SetRenderTarget(target);
            _renderAPI.ClearViewport(clearFlags, viewport->GetClearColorValue(),
                viewport->GetClearDepthValue(), viewport->GetClearStencilValue());
        }
        else
        {
            _renderAPI.SetRenderTarget(target, 0);
        }

        _renderAPI.SetViewport(viewport->GetArea());

        // The only overlay we can manage currently
        if(view.GetSceneCamera()->IsMain() && GuiAPI::Instance().IsGuiInitialized())
        {
            GuiAPI::Instance().EndFrame();
        }

        view.EndFrame();

        return true;
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

    void RenderMan::NotifyCamerasCleared()
    {
        _scene->ClearCameras();
    }

    void RenderMan::NotifyRenderableAdded(Renderable* renderable)
    {
        _scene->RegisterRenderable(renderable);
    }

    void RenderMan::NotifyRenderableUpdated(Renderable* renderable)
    {
        _scene->UpdateRenderable(renderable);
    }

    void RenderMan::NotifyRenderableRemoved(Renderable* renderable)
    {
        _scene->UnregisterRenderable(renderable);
    }

    void RenderMan::NotifyLightsCleared()
    {
        _scene->ClearLights();
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

    void RenderMan::NotifyRenderablesCleared()
    {
        _scene->ClearRenderables();
    }

    void RenderMan::NotifySkyboxAdded(Skybox* skybox)
    {
        _scene->RegisterSkybox(skybox);
    }

    void RenderMan::NotifySkyboxRemoved(Skybox* skybox)
    {
        _scene->UnregisterSkybox(skybox);
    }

    void RenderMan::NotifySkyboxCleared()
    {
        _scene->ClearSkybox();
    }

    void RenderMan::NotifyDecalAdded(Decal* decal)
    {
        _scene->RegisterDecal(decal);
    }

    void RenderMan::NotifyDecalUpdated(Decal* decal)
    {
        _scene->UpdateDecal(decal);
    }

    void RenderMan::NotifyDecalRemoved(Decal* decal)
    {
        _scene->UnregisterDecal(decal);
    }

    void RenderMan::NotifyDecalsCleared()
    {
        _scene->ClearDecals();
    }

    void RenderMan::BatchRenderables()
    {
        _scene->BatchRenderables();
    }

    void RenderMan::DestroyBatchedRenderables()
    {
        _scene->DestroyBatchedRenderables();
    }

    void RenderMan::SetLastRenderTexture(RenderOutputType type, SPtr<Texture> renderTexture) const
    {
        switch (type)
        {
        case RenderOutputType::Final:
            _renderTextures.FinalTex = renderTexture;
            break;
        case RenderOutputType::Color:
            _renderTextures.ColorTex = renderTexture;
            break;
        case RenderOutputType::Normal:
            _renderTextures.NormalTex = renderTexture;
            break;
        case RenderOutputType::Depth:
            _renderTextures.DepthTex = renderTexture;
            break;
        case RenderOutputType::Emissive:
            _renderTextures.EmissiveTex = renderTexture;
            break;
        case RenderOutputType::Velocity:
            _renderTextures.VelocityTex = renderTexture;
            break;
        case RenderOutputType::SSAO:
            _renderTextures.SSAOTex = renderTexture;
            break;
        }
    }

    SPtr<Texture> RenderMan::GetLastRenderTexture(RenderOutputType type) const
    {
        switch (type)
        {
        case RenderOutputType::Final:
            return _renderTextures.FinalTex;
            break;
        case RenderOutputType::Color:
            return _renderTextures.ColorTex;
            break;
        case RenderOutputType::Normal:
            return _renderTextures.NormalTex;
            break;
        case RenderOutputType::Depth:
            return _renderTextures.DepthTex;
            break;
        case RenderOutputType::Emissive:
            return _renderTextures.EmissiveTex;
            break;
        case RenderOutputType::Velocity:
            return _renderTextures.VelocityTex;
            break;
        case RenderOutputType::SSAO:
            return _renderTextures.SSAOTex;
            break;
        }

        return nullptr;
    }

    void RenderMan::SetLastShadowMapTexture(Light* light, SPtr<Texture> depthBuffer) const
    {
        // TODO
    }

    SPtr<Texture> RenderMan::GetLastShadowMapTexture(SPtr<Light> light) const
    {
        return nullptr;
    }

    SPtr<RenderMan> gRenderMan()
    {
        return std::static_pointer_cast<RenderMan>(gRenderer());
    }
}
