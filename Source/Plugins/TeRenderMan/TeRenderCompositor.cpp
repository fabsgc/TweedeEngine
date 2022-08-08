#include "TeRenderCompositor.h"

#include "RenderAPI/TeRenderAPI.h"
#include "TeRenderMan.h"
#include "TeRendererView.h"
#include "TeRendererLight.h"
#include "TeRendererScene.h"
#include "TeRendererTextures.h"
#include "TeRenderManOptions.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeSkybox.h"
#include "Renderer/TeTextureDownsampleMat.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Utility/TeFrameAllocator.h"
#include "Gui/TeGuiAPI.h"
#include "Mesh/TeMesh.h"
#include "Image/TePixelUtil.h"
#include "PostProcessing/TeFXAAMat.h"
#include "PostProcessing/TeSkyboxMat.h"
#include "PostProcessing/TeToneMappingMat.h"
#include "PostProcessing/TeBloomMat.h"
#include "PostProcessing/TeMotionBlurMat.h"
#include "PostProcessing/TeGaussianBlurMat.h"

namespace te
{
    UnorderedMap<String, RenderCompositor::NodeType*> RenderCompositor::_nodeTypes;

    /** Renders all elements in a render queue. */
    void RenderQueueElements(const Vector<RenderQueueElement>& elements, const RendererView& view, const SceneInfo& scene, const RendererViewGroup& viewGroup)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<Material> lastMaterial = nullptr;
        UINT32 gpuParamsBindFlags = 0;

        static const Vector<String> PerLightBuffer = { "PerLightsBuffer" };
        static const Vector<String> PerCameraBuffer = { "PerCameraBuffer" };
        static const Vector<String> PerFrameBuffer = { "PerFrameBuffer" };

        for(auto& entry : elements)
        {
            rapi.PushMarker("[DRAW] Renderable", Color(0.7f, 0.8f, 0.2f));

            if(entry.ApplyPass)
                gRendererUtility().SetPass(entry.RenderElem->MaterialElem, entry.TechniqueIdx, entry.PassIdx);

            // If Material is the same as the previous object, we only set constant buffer params
            // Instead, we set full gpu params
            // We also set camera buffer view here (because it will set PerCameraBuffer correctly for the current pass on this material only once)
            if (!lastMaterial || lastMaterial != entry.RenderElem->MaterialElem)
            {
                // If Globall Illumination is enabled and if a Skybox with a texture exists,
                // We bind this texture for this material
                if (entry.RenderElem->MaterialElem->GetProperties().DoIndirectLighting)
                {
                    if (view.GetRenderSettings().EnableSkybox)
                    {
                        Skybox* skybox = scene.SkyboxElem;
                        SPtr<Texture> diffuseIrradiance = skybox ? skybox->GetDiffuseIrradiance() : nullptr;
                        SPtr<Texture> specularIrradiance = skybox ? skybox->GetSpecularIrradiance() : nullptr;

                        entry.RenderElem->GpuParamsElem[entry.PassIdx]->SetTexture("DiffuseIrrMap", diffuseIrradiance);
                        entry.RenderElem->GpuParamsElem[entry.PassIdx]->SetTexture("PrefilteredRadianceMap", specularIrradiance);
                    }
                }

                // We bind PreIntegratedEnvGF
                {
                    if(RendererTextures::PreIntegratedEnvGF)
                        entry.RenderElem->GpuParamsElem[entry.PassIdx]->SetTexture("PreIntegratedEnvGF", RendererTextures::PreIntegratedEnvGF);
                }

                gpuParamsBindFlags = GPU_BIND_ALL;
                lastMaterial = entry.RenderElem->MaterialElem;

                entry.RenderElem->GpuParamsElem[entry.PassIdx]
                    ->SetParamBlockBuffer("PerLightsBuffer", gPerLightsParamBuffer);

                rapi.SetGpuParams(entry.RenderElem->GpuParamsElem[entry.PassIdx],
                    GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_LISTED, PerLightBuffer);

                entry.RenderElem->GpuParamsElem[entry.PassIdx]
                    ->SetParamBlockBuffer("PerCameraBuffer", view.GetPerViewBuffer());

                rapi.SetGpuParams(entry.RenderElem->GpuParamsElem[entry.PassIdx],
                     GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_LISTED, PerCameraBuffer);

                entry.RenderElem->GpuParamsElem[entry.PassIdx]
                    ->SetParamBlockBuffer("PerFrameBuffer", scene.PerFrameParamBuffer);

                rapi.SetGpuParams(entry.RenderElem->GpuParamsElem[entry.PassIdx],
                    GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_LISTED, PerFrameBuffer);

                entry.RenderElem->MaterialElem->SetGpuParam(entry.RenderElem->GpuParamsElem[entry.PassIdx]);
            }
            else
            {
                gpuParamsBindFlags = GPU_BIND_PARAM_BLOCK | GPU_BIND_BUFFER;
            }

            bool isInstanced = (entry.RenderElem->InstanceCount > 0) ? true : false;

            gRendererUtility().SetPassParams(entry.RenderElem->GpuParamsElem[entry.PassIdx], gpuParamsBindFlags, isInstanced);
            entry.RenderElem->Draw();

            rapi.PopMarker();
        }
    }

    RenderCompositor::~RenderCompositor()
    {
        _nodeBackup.clear();

        for (auto& entry : _nodeInfos)
            te_delete(entry.Node);

        Clear();
    }

    void RenderCompositor::Build(const RendererView& view, const String& finalNode)
    {
       Clear();

        UnorderedMap<String, UINT32> processedNodes;
        _isValid = true;

        std::function<bool(const String&)> registerNode = [&](const String& nodeId)
        {
            // Find node type
            auto iterFind = _nodeTypes.find(nodeId);
            if (iterFind == _nodeTypes.end())
            {
                TE_DEBUG("Cannot find render compositor node of type \"{" + String(nodeId.c_str()) + "}\".");
                return false;
            }

            NodeType* nodeType = iterFind->second;

            // Register current node
            auto iterFind2 = processedNodes.find(nodeId);

            // New node
            if (iterFind2 == processedNodes.end())
            {
                // Mark it as invalid for now
                processedNodes[nodeId] = static_cast<UINT32>(-1);
            }

            // Register node dependencies
            Vector<String> depIds = nodeType->GetDependencies(view);
            for (auto& dep : depIds)
            {
                if (!registerNode(dep))
                    return false;
            }

            // Register current node
            UINT32 curIdx;

            // New node, properly populate its index
            if (iterFind2 == processedNodes.end())
            {
                iterFind2 = processedNodes.find(nodeId);

                curIdx = (UINT32)_nodeInfos.size();
                _nodeInfos.push_back(NodeInfo());
                processedNodes[nodeId] = curIdx;

                NodeInfo& nodeInfo = _nodeInfos.back();
                nodeInfo.Type = nodeType;
                nodeInfo.LastUseIdx = static_cast<UINT32>(-1);

                // If a node has already been created, we reuse the instance
                // instead of creating a new one
                auto iterFind3 = _nodeBackup.find(nodeType);
                if (iterFind3 == _nodeBackup.end())
                {
                    nodeInfo.Node = nodeType->Create();
                    _nodeBackup[nodeType] = nodeInfo.Node;
                }
                else
                {
                    nodeInfo.Node = iterFind3->second;
                }

                for (auto& depId : depIds)
                {
                    iterFind2 = processedNodes.find(depId);

                    NodeInfo& depNodeInfo = _nodeInfos[iterFind2->second];
                    nodeInfo.Inputs.push_back(depNodeInfo.Node);
                }
            }
            else // Existing node
            {
                curIdx = iterFind2->second;

                // Check if invalid
                if (curIdx == (UINT32)-1)
                {
                    TE_DEBUG("Render compositor nodes recursion detected. Node \"{" + String(nodeId.c_str()) + "}\" "
                        "depends on node \"{" + String(iterFind->first.c_str()) + "}\" which is not available at this stage.");
                    return false;
                }
            }

            // Update dependency last use counters
            for (auto& dep : depIds)
            {
                iterFind2 = processedNodes.find(dep);

                NodeInfo& depNodeInfo = _nodeInfos[iterFind2->second];
                if (depNodeInfo.LastUseIdx == (UINT32)-1)
                    depNodeInfo.LastUseIdx = curIdx;
                else
                    depNodeInfo.LastUseIdx = std::max(depNodeInfo.LastUseIdx, curIdx);
            }

            return true;
        };

        _isValid = registerNode(finalNode);

        if (!_isValid)
            Clear();
    }

    void RenderCompositor::Execute(RenderCompositorNodeInputs& inputs) const
    { 
        if (!_isValid)
            return;

        te_frame_mark();
        {
            FrameVector<const NodeInfo*> activeNodes;

            UINT32 idx = 0;
            for (auto& entry : _nodeInfos)
            {
                inputs.InputNodes = entry.Inputs;
                entry.Node->Render(inputs);

                activeNodes.push_back(&entry);

                for (UINT32 i = 0; i < (UINT32)activeNodes.size(); ++i)
                {
                    if (activeNodes[i] == nullptr)
                        continue;

                    if (activeNodes[i]->LastUseIdx <= idx)
                    {
                        activeNodes[i]->Node->Clear();
                        activeNodes[i] = nullptr;
                    }
                }

                idx++;
            }
        }
        te_frame_clear();

        if (!_nodeInfos.empty())
            _nodeInfos.back().Node->Clear();
    }

    void RenderCompositor::Clear()
    {
        _nodeInfos.clear();
        _isValid = false;
    }

    // ############# GPU INITIALIZATION

    void RCNodeGpuInitializationPass::Render(const RenderCompositorNodeInputs& inputs)
    {
        // Allocate necessary textures & targets
        GpuResourcePool& resPool = gGpuResourcePool();
        const RendererViewProperties& viewProps = inputs.View.GetProperties();

        const UINT32 width = viewProps.Target.ViewRect.width;
        const UINT32 height = viewProps.Target.ViewRect.height;
        const UINT32 numSamples = viewProps.Target.NumSamples;

        bool needsVelocity = inputs.View.RequiresVelocityWrites();

        // Note: Consider customizable formats. e.g. for testing if quality can be improved with higher precision normals.
        SceneTex = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA16F, width, height, TU_RENDERTARGET,
            numSamples, true));
        NormalTex = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA8, width, height, TU_RENDERTARGET,
            numSamples, true));
        EmissiveTex = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA8, width, height, TU_RENDERTARGET,
            numSamples, true));
        if (needsVelocity)
        {
            VelocityTex = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA8, width, height, TU_RENDERTARGET,
                numSamples, false));
        }

        DepthTex = gGpuResourcePool().Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_D32_S8X24, width, height, TU_DEPTHSTENCIL,
            numSamples, false));

        bool rebuildRT = false;
        if (RenderTargetTex != nullptr)
        {
            UINT32 targetIdx = 0;
            rebuildRT |= RenderTargetTex->GetColorTexture(targetIdx++) != SceneTex->Tex;
            rebuildRT |= RenderTargetTex->GetColorTexture(targetIdx++) != NormalTex->Tex;
            rebuildRT |= RenderTargetTex->GetColorTexture(targetIdx++) != EmissiveTex->Tex;
            if (needsVelocity)
                rebuildRT |= RenderTargetTex->GetColorTexture(targetIdx++) != VelocityTex->Tex;
            rebuildRT |= RenderTargetTex->GetDepthStencilTexture() != DepthTex->Tex;
        }
        else
            rebuildRT = true;

        if (RenderTargetTex == nullptr || rebuildRT)
        {
            UINT32 targetIdx = 0;

            RENDER_TEXTURE_DESC gbufferDesc;
            gbufferDesc.ColorSurfaces[targetIdx].Tex = SceneTex->Tex;
            gbufferDesc.ColorSurfaces[targetIdx].Face = 0;
            gbufferDesc.ColorSurfaces[targetIdx].NumFaces = 1;
            gbufferDesc.ColorSurfaces[targetIdx].MipLevel = 0;
            targetIdx++;

            gbufferDesc.ColorSurfaces[targetIdx].Tex = NormalTex->Tex;
            gbufferDesc.ColorSurfaces[targetIdx].Face = 0;
            gbufferDesc.ColorSurfaces[targetIdx].NumFaces = 1;
            gbufferDesc.ColorSurfaces[targetIdx].MipLevel = 0;
            targetIdx++;

            gbufferDesc.ColorSurfaces[targetIdx].Tex = EmissiveTex->Tex;
            gbufferDesc.ColorSurfaces[targetIdx].Face = 0;
            gbufferDesc.ColorSurfaces[targetIdx].NumFaces = 1;
            gbufferDesc.ColorSurfaces[targetIdx].MipLevel = 0;
            targetIdx++;

            if (needsVelocity)
            {
                gbufferDesc.ColorSurfaces[targetIdx].Tex = VelocityTex->Tex;
                gbufferDesc.ColorSurfaces[targetIdx].Face = 0;
                gbufferDesc.ColorSurfaces[targetIdx].NumFaces = 1;
                gbufferDesc.ColorSurfaces[targetIdx].MipLevel = 0;
                targetIdx++;
            }

            gbufferDesc.DepthStencilSurface.Tex = DepthTex->Tex;
            gbufferDesc.DepthStencilSurface.Face = 0;
            gbufferDesc.DepthStencilSurface.MipLevel = 0;

            RenderTargetTex = RenderTexture::Create(gbufferDesc);
        }

        Vector3I lightCounts;
        const PerLightData* lights[STANDARD_FORWARD_MAX_NUM_LIGHTS];
        const auto numRenderables = (UINT32)inputs.Scene.Renderables.size();
        for (UINT32 i = 0; i < numRenderables; i++)
        {
            if (!inputs.Scene.Renderables[i]->RenderablePtr->GetCastLights())
                continue;

            // Compute list of lights that influence renderables
            const Bounds& bounds = inputs.Scene.RenderableCullInfos[i].Boundaries;
            inputs.ViewGroup.GetVisibleLightData().GatherInfluencingLights(bounds, lights, lightCounts);
        }

        PerLightsBuffer::UpdatePerLights(lights, lightCounts.x + lightCounts.y + lightCounts.z);
    }

    void RCNodeGpuInitializationPass::Clear()
    {
        SceneTex = nullptr;
        NormalTex = nullptr;
        EmissiveTex = nullptr;
        VelocityTex = nullptr;
        DepthTex = nullptr;
    }

    Vector<String> RCNodeGpuInitializationPass::GetDependencies(const RendererView& view)
    {
        return { };
    }

    // ############# FORWARD PASS

    void RCNodeForwardPass::Render(const RenderCompositorNodeInputs& inputs)
    {
        RenderQueue* opaqueElements = inputs.View.GetOpaqueQueue().get();
        const Vector<RenderQueueElement> elements = opaqueElements->GetSortedElements();
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        //if (inputs.Scene.SkyboxElem)
        //    inputs.Scene.SkyboxElem->FilterTexture();

        inputs.CurrRenderAPI.PushMarker("[DRAW] Forward Pass", Color(0.8f, 0.6f, 0.8f));

        Camera* sceneCamera = inputs.View.GetSceneCamera();
        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;

        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex);
        inputs.CurrRenderAPI.ClearViewport(clearBuffers, Color::Black);

        if (elements.size() > 0)
        {
            // Render all opaque elements     
            RenderQueueElements(elements, inputs.View, inputs.Scene, inputs.ViewGroup);
        }

        if (sceneCamera != nullptr)
            inputs.View.NotifyCompositorTargetChanged(gpuInitializationPassNode->RenderTargetTex);

        // Make sure that any compute shaders are able to read g-buffer by unbinding it
        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeForwardPass::Clear()
    { }

    Vector<String> RCNodeForwardPass::GetDependencies(const RendererView& view)
    {
        return { RCNodeGpuInitializationPass::GetNodeId() };
    }

    // ############# SKYBOX

    void RCNodeSkybox::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Skybox", Color(0.4f, 0.8f, 0.1f));

        Skybox* skybox = nullptr;
        if (inputs.Scene.SkyboxElem && inputs.View.GetRenderSettings().EnableSkybox)
            skybox = inputs.Scene.SkyboxElem;

        SPtr<Texture> radiance = skybox ? skybox->GetTexture() : nullptr;
        float brightness = skybox ? skybox->GetBrightness() : 1.0f;

        if (radiance != nullptr)
        {
            SkyboxMat* material = SkyboxMat::Get();
            material->Bind(inputs.View.GetPerViewBuffer(), radiance, Color::White, brightness);
        }
        else
        {
            SkyboxMat* material = SkyboxMat::Get();
            Color clearColor = inputs.View.GetProperties().Target.ClearColor.GetLinear();
            material->Bind(inputs.View.GetPerViewBuffer(), nullptr, clearColor, brightness);
        }

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        int readOnlyFlags = FBT_DEPTH | FBT_STENCIL;

        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex, readOnlyFlags);

        SPtr<Mesh> mesh = gRendererUtility().GetSkyBoxMesh();
        gRendererUtility().Draw(mesh, mesh->GetProperties().GetSubMesh(0));

        Camera* sceneCamera = inputs.View.GetSceneCamera();
        if (sceneCamera != nullptr)
            inputs.View.NotifyCompositorTargetChanged(gpuInitializationPassNode->RenderTargetTex);

        // Make sure that any compute shaders are able to read g-buffer by unbinding it
        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeSkybox::Clear()
    { }

    Vector<String> RCNodeSkybox::GetDependencies(const RendererView& view)
    {
        return { 
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeForwardPass::GetNodeId() 
        };
    }

    // ############# FORWARD TRANSPARENT PASS

    void RCNodeForwardTransparentPass::Render(const RenderCompositorNodeInputs& inputs)
    {
        RenderQueue* transparentElements = inputs.View.GetTransparentQueue().get();
        const Vector<RenderQueueElement> elements = transparentElements->GetSortedElements();

        if (elements.size() == 0)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Forward Transparent Pass", Color(0.6f, 0.6f, 0.9f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        int readOnlyFlags = FBT_DEPTH;

        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex, readOnlyFlags);

        Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
        inputs.CurrRenderAPI.SetViewport(area);

        RenderQueueElements(elements, inputs.View, inputs.Scene, inputs.ViewGroup);

        Camera* sceneCamera = inputs.View.GetSceneCamera();
        if (sceneCamera != nullptr)
            inputs.View.NotifyCompositorTargetChanged(gpuInitializationPassNode->RenderTargetTex);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeForwardTransparentPass::Clear()
    { }

    Vector<String> RCNodeForwardTransparentPass::GetDependencies(const RendererView& view)
    {
        return { 
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeSkybox::GetNodeId() 
        };
    }

    // ############# SCREEN SPACE

    void RCNodeHalfSceneColor::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Half Scene Color", Color(0.74f, 0.21f, 0.32f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        SPtr<Texture> input = gpuInitializationPassNode->EmissiveTex->Tex;
        TextureDownsampleMat* downsampleMat = TextureDownsampleMat::Get();
        const TextureProperties& rtProps = input->GetProperties();

        Output = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                rtProps.GetFormat(), 
                rtProps.GetWidth() / 2, 
                rtProps.GetHeight() / 2, 
                TU_RENDERTARGET)
        );

        downsampleMat->Execute(input, 0, Output->RenderTex);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeHalfSceneColor::Clear()
    {
        Output = nullptr;
    }

    Vector<String> RCNodeHalfSceneColor::GetDependencies(const RendererView& view)
    {
        return { 
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    constexpr UINT32 RCNodeSceneColorDownsamples::MAX_NUM_DOWNSAMPLES;

    void RCNodeSceneColorDownsamples::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Scene Scene Down Samples", Color(0.44f, 0.71f, 0.52f));

        GpuResourcePool& resPool = gGpuResourcePool();
        RCNodeHalfSceneColor* halfSceneColorNode = static_cast<RCNodeHalfSceneColor*>(inputs.InputNodes[2]);
        const TextureProperties& halfSceneProps = halfSceneColorNode->Output->Tex->GetProperties();

        const UINT32 totalDownsampleLevels = PixelUtil::GetMaxMipmaps(
            halfSceneProps.GetWidth(),
            halfSceneProps.GetHeight(),
            1) + 1;

        AvailableDownsamples = Math::Min(MAX_NUM_DOWNSAMPLES, totalDownsampleLevels);

        {
            Outputs[0] = halfSceneColorNode->Output;

            TextureDownsampleMat* downsampleMat = TextureDownsampleMat::Get();
            for (UINT32 i = 1; i < AvailableDownsamples; i++)
            {
                const TextureProperties& rtProps = Outputs[i - 1]->Tex->GetProperties();

                Outputs[i] = resPool.Get(
                    POOLED_RENDER_TEXTURE_DESC::Create2D(
                        rtProps.GetFormat(),
                        rtProps.GetWidth() / 2,
                        rtProps.GetHeight() / 2, 
                        TU_RENDERTARGET
                    )
                );

                downsampleMat->Execute(Outputs[i - 1]->Tex, 0, Outputs[i]->RenderTex);
            }
        }

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeSceneColorDownsamples::Clear()
    { 
        for (UINT32 i = 0; i < MAX_NUM_DOWNSAMPLES; i++)
            Outputs[i] = nullptr;
    }

    Vector<String> RCNodeSceneColorDownsamples::GetDependencies(const RendererView& view)
    {
        return {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId(),
            RCNodeHalfSceneColor::GetNodeId()
        };
    }

    void RCNodeResolvedSceneDepth::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Resolve Scene Depth", Color(0.2f, 0.2f, 0.8f));

        const RendererViewProperties& viewProps = inputs.View.GetProperties();
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        if (viewProps.Target.NumSamples > 1)
        {
            UINT32 width = viewProps.Target.ViewRect.width;
            UINT32 height = viewProps.Target.ViewRect.height;

            Output = gGpuResourcePool().Get(
                POOLED_RENDER_TEXTURE_DESC::Create2D(PF_D32_S8X24, width, height, TU_DEPTHSTENCIL, 1, false));

            inputs.CurrRenderAPI.SetRenderTarget(Output->RenderTex);
            inputs.CurrRenderAPI.ClearRenderTarget(FBT_STENCIL);
            gRendererUtility().Blit(gpuInitializationPassNode->DepthTex->Tex, Rect2I::EMPTY, false, true);
        }
        else
        {
            Output = gpuInitializationPassNode->DepthTex;
        }

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeResolvedSceneDepth::Clear()
    {
        Output = nullptr;
    }

    Vector<String> RCNodeResolvedSceneDepth::GetDependencies(const RendererView& view)
    {
        return 
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeSceneColorDownsamples::GetNodeId() 
        };
    }

    // ############# POST PROCESS

    void RCNodePostProcess::GetAndSwitch(const RendererView& view, SPtr<RenderTexture>& output, SPtr<Texture>& lastFrame) const
    {
        const RendererViewProperties& viewProps = view.GetProperties();
        UINT32 width = viewProps.Target.ViewRect.width;
        UINT32 height = viewProps.Target.ViewRect.height;
        UINT32 samples = viewProps.Target.NumSamples;

        if (!_output[_currentIdx])
        {
            _output[_currentIdx] = gGpuResourcePool().Get(
                POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA16F, width, height, TU_RENDERTARGET, samples, false));
        }

        output = _output[_currentIdx]->RenderTex;

        UINT32 otherIdx = (_currentIdx + 1) % 2;
        if (_output[otherIdx])
            lastFrame = _output[otherIdx]->Tex;

        _currentIdx = otherIdx;
    }

    SPtr<Texture> RCNodePostProcess::GetLastOutput() const
    {
        UINT32 otherIdx = (_currentIdx + 1) % 2;
        if (_output[otherIdx])
            return _output[otherIdx]->Tex;

        return nullptr;
    }

    void RCNodePostProcess::Render(const RenderCompositorNodeInputs& inputs)
    { }

    void RCNodePostProcess::Clear()
    {
        _output[0] = nullptr;
        _output[1] = nullptr;
        _currentIdx = 0;
    }

    Vector<String> RCNodePostProcess::GetDependencies(const RendererView& view)
    {
        return { RCNodeForwardTransparentPass::GetNodeId() };
    }

    // ############# TONE MAPPING

    void RCNodeTonemapping::Render(const RenderCompositorNodeInputs& inputs)
    {
        const RenderSettings& settings = inputs.View.GetRenderSettings();
        if (!settings.EnableHDR)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Tone Mapping", Color(0.7f, 0.9f, 0.4f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[1]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        ToneMappingMat* toneMapping = ToneMappingMat::Get();

        if (!ppLastFrame)
            ppLastFrame = gpuInitializationPassNode->SceneTex->Tex;

        auto& texProps = ppLastFrame->GetProperties();
        toneMapping->Execute(ppLastFrame, ppOutput, texProps.GetNumSamples(),
            settings.Gamma, settings.ExposureScale, settings.Contrast, settings.Brightness, !settings.Tonemapping.Enabled);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeTonemapping::Clear()
    { }

    Vector<String> RCNodeTonemapping::GetDependencies(const RendererView& view)
    {
        Vector<String> deps = {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };

        if(view.GetRenderSettings().MotionBlur.Enabled)
            deps.push_back(RCNodeMotionBlur::GetNodeId());

        if(view.GetRenderSettings().Bloom.Enabled)
            deps.push_back(RCNodeBloom::GetNodeId());

        if(view.GetRenderSettings().AmbientOcclusion.Enabled)
            deps.push_back(RCNodeSSAO::GetNodeId());

        if(view.GetRenderSettings().DepthOfField.Enabled)
            deps.push_back(RCNodeGaussianDOF::GetNodeId());

        return deps;
    }

    // ############# MOTION BLUR

    void RCNodeMotionBlur::Render(const RenderCompositorNodeInputs& inputs)
    {
        const MotionBlurSettings& settings = inputs.View.GetRenderSettings().MotionBlur;
        if (!settings.Enabled)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Motion Blur", Color(0.1f, 0.9f, 0.6f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[1]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        SPtr<Texture> depth = gpuInitializationPassNode->DepthTex->Tex;
        SPtr<Texture> velocity = gpuInitializationPassNode->VelocityTex->Tex;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        MotionBlurMat* motionBlur = MotionBlurMat::Get();

        if (!ppLastFrame)
            ppLastFrame = gpuInitializationPassNode->SceneTex->Tex;
        
        auto& texProps = ppLastFrame->GetProperties();
        motionBlur->Execute(ppLastFrame, ppOutput, depth, velocity, inputs.View.GetPerViewBuffer(),
            settings, texProps.GetNumSamples());

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeMotionBlur::Clear()
    { }

    Vector<String> RCNodeMotionBlur::GetDependencies(const RendererView& view)
    {
        return
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    // ############# GAUSSIAN DOF

    void RCNodeGaussianDOF::Render(const RenderCompositorNodeInputs& inputs)
    { }

    void RCNodeGaussianDOF::Clear()
    { }

    Vector<String> RCNodeGaussianDOF::GetDependencies(const RendererView& view)
    {
        return
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    // ############# FXAA

    void RCNodeFXAA::Render(const RenderCompositorNodeInputs& inputs)
    {
        const RenderSettings& settings = inputs.View.GetRenderSettings();
        if (settings.AntialiasingAglorithm != AntiAliasingAlgorithm::FXAA)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] FXAA", Color(0.35f, 0.35f, 0.7f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[1]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        FXAAMat* fxaa = FXAAMat::Get();

        if(ppLastFrame)
            fxaa->Execute(ppLastFrame, ppOutput);
        else
            fxaa->Execute(gpuInitializationPassNode->SceneTex->Tex, ppOutput);

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeFXAA::Clear()
    { }

    Vector<String> RCNodeFXAA::GetDependencies(const RendererView& view)
    {
        Vector<String> deps = {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };

        if(view.GetRenderSettings().MotionBlur.Enabled)
            deps.push_back(RCNodeMotionBlur::GetNodeId());

        if(view.GetRenderSettings().Bloom.Enabled)
            deps.push_back(RCNodeBloom::GetNodeId());

        if(view.GetRenderSettings().AmbientOcclusion.Enabled)
            deps.push_back(RCNodeSSAO::GetNodeId());

        if(view.GetRenderSettings().DepthOfField.Enabled)
            deps.push_back(RCNodeGaussianDOF::GetNodeId());

        return deps;
    }

    // ############# TAA

    void RCNodeTemporalAA::Render(const RenderCompositorNodeInputs& inputs)
    {
        const RenderSettings& settings = inputs.View.GetRenderSettings();
        if (settings.AntialiasingAglorithm != AntiAliasingAlgorithm::TAA)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] TAA", Color(0.55f, 0.85f, 0.25f));

        /*RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[3]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);*/

        // TODO temporal AA

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeTemporalAA::Clear()
    { }

    Vector<String> RCNodeTemporalAA::GetDependencies(const RendererView& view)
    {
        Vector<String> deps = {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };

        if(view.GetRenderSettings().MotionBlur.Enabled)
            deps.push_back(RCNodeMotionBlur::GetNodeId());

        if(view.GetRenderSettings().Bloom.Enabled)
            deps.push_back(RCNodeBloom::GetNodeId());

        if(view.GetRenderSettings().AmbientOcclusion.Enabled)
            deps.push_back(RCNodeSSAO::GetNodeId());

        if(view.GetRenderSettings().DepthOfField.Enabled)
            deps.push_back(RCNodeGaussianDOF::GetNodeId());

        return deps;
    }

    // ############# SSAO

    void RCNodeSSAO::Render(const RenderCompositorNodeInputs& inputs)
    { 
        /** Maximum valid depth range within samples in a sample set. In meters. */
        static const float DEPTH_RANGE = 1.0f;

        const AmbientOcclusionSettings& settings = inputs.View.GetRenderSettings().AmbientOcclusion;
        if (!settings.Enabled)
        {
            Output = Texture::WHITE;
            return;
        }

        inputs.CurrRenderAPI.PushMarker("[DRAW] SSAO", Color(0.25f, 0.35f, 0.95f));

        Output = Texture::WHITE; // TODO

        // GpuResourcePool& resPool = gGpuResourcePool();
        // const RendererViewProperties& viewProps = inputs.View.GetProperties();

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeSSAO::Clear()
    { 
        Output = nullptr;
    }

    Vector<String> RCNodeSSAO::GetDependencies(const RendererView& view)
    {
        return
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeResolvedSceneDepth::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    // ############# BLOOM

    void RCNodeBloom::Render(const RenderCompositorNodeInputs& inputs)
    {
        const RendererViewProperties& viewProps = inputs.View.GetProperties();
        const RenderSettings& settings = inputs.View.GetRenderSettings();
        if (!settings.Bloom.Enabled)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Bloom", Color(0.85f, 0.55f, 0.15f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        RCNodeSceneColorDownsamples* sceneColorDownSampleNode = static_cast<RCNodeSceneColorDownsamples*>(inputs.InputNodes[1]);
        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[2]);

        constexpr UINT32 PREFERRED_NUM_DOWNSAMPLE_LEVELS = 3;
        constexpr UINT32 NUM_STEPS_PER_QUALITY[] = { 1, 1, 2, 3 };
        constexpr UINT32 OUTPUT_DOWN_SAMPLE_FACTOR_PER_QUALITY[] = { 3, 2, 1, 1 };

        const UINT32 availableDownsamples = sceneColorDownSampleNode->AvailableDownsamples;
        const UINT32 numDownsamples = Math::Min(availableDownsamples, PREFERRED_NUM_DOWNSAMPLE_LEVELS);
        assert(numDownsamples >= 1);

        // ### First, we get emissive texture from forward pass, use it as input for our GaussianBlur material
        // ### and create a new tex representing the blured result
        GaussianBlurMat* gaussianBlur = GaussianBlurMat::Get();
        SPtr<PooledRenderTexture> emissiveTex = gpuInitializationPassNode->EmissiveTex;

        // We can reduce blur texture size according to bloom quality
        const UINT32 quality = Math::Clamp((UINT32)settings.Bloom.Quality, 0U, 3U);
        const UINT32 numSteps = NUM_STEPS_PER_QUALITY[quality];
        const UINT32 downSampleFactor = OUTPUT_DOWN_SAMPLE_FACTOR_PER_QUALITY[quality];

        const TextureProperties& inputProps = emissiveTex->Tex->GetProperties();
        SPtr<PooledRenderTexture> blurOutput = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                inputProps.GetFormat(),
                inputProps.GetWidth() / downSampleFactor,
                inputProps.GetHeight() / downSampleFactor,
                TU_RENDERTARGET,
                viewProps.Target.NumSamples
            )
        );

        gaussianBlur->Execute(emissiveTex->Tex, blurOutput->RenderTex, settings.Bloom.FilterSize,
            settings.Bloom.Tint, emissiveTex->Tex->GetProperties().GetNumSamples());

        /*SPtr<PooledRenderTexture> prevOutput;
        for (UINT32 i = 0; i < numSteps; i++)
        {
            const SPtr<PooledRenderTexture> downsampledTex = sceneColorDownSampleNode->Outputs[i];
            SPtr<PooledRenderTexture> blurInput = downsampledTex;

            gaussianBlur->Execute(blurInput->Tex, blurOutput->RenderTex, settings.Bloom.FilterSize,
                settings.Bloom.Tint, blurInput->Tex->GetProperties().GetNumSamples());
        }*/

        /*UINT32 blurTextureFactor = 2;

        // We can reduce blur texture size according to bloom quality
        if (settings.Bloom.Quality == BloomQuality::Ultra)
            blurTextureFactor = 1;
        else if (settings.Bloom.Quality == BloomQuality::High)
            blurTextureFactor = 2;
        else if (settings.Bloom.Quality == BloomQuality::Medium)
            blurTextureFactor = 3;
        else if (settings.Bloom.Quality == BloomQuality::Low)
            blurTextureFactor = 4;

        const TextureProperties& inputProps = emissiveTex->Tex->GetProperties();
        SPtr<PooledRenderTexture> blurOutput = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                inputProps.GetFormat(),
                inputProps.GetWidth() / blurTextureFactor,
                inputProps.GetHeight() / blurTextureFactor,
                TU_RENDERTARGET,
                viewProps.Target.NumSamples
            )
        );

        gaussianBlur->Execute(emissiveTex->Tex, blurOutput->RenderTex, settings.Bloom.FilterSize,
            settings.Bloom.Tint, viewProps.Target.NumSamples);*/

        // ### Once we have our blured texture, we call our bloom material which will add this blured texture to the 
        // ### output final texture
        BloomMat* bloom = BloomMat::Get();
        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        if (!ppLastFrame)
            ppLastFrame = gpuInitializationPassNode->SceneTex->Tex;

        auto& texProps = ppLastFrame->GetProperties();
        bloom->Execute(ppLastFrame, ppOutput, blurOutput->Tex,
            settings.Bloom.Intensity, texProps.GetNumSamples());

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeBloom::Clear()
    { }

    Vector<String> RCNodeBloom::GetDependencies(const RendererView& view)
    {
        return
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeSceneColorDownsamples::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    // ############# FINAL RENDER

    void RCNodeFinalResolve::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Final", Color(0.6f, 0.7f, 0.8f));

        const RendererViewProperties& viewProps = inputs.View.GetProperties();

        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[4]);
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        RCNodeSSAO* SSAONode = nullptr;

        SPtr<Texture> input;
        if (viewProps.RunPostProcessing && viewProps.Target.NumSamples == 1)
        {
            SSAONode = static_cast<RCNodeSSAO*>(inputs.InputNodes[7]);

            switch (inputs.View.GetSceneCamera()->GetRenderSettings()->OutputType)
            {
            case RenderOutputType::Final:
                input = postProcessNode->GetLastOutput();
                break;
            case RenderOutputType::Color:
                input = gpuInitializationPassNode->SceneTex->Tex;
                break;
            case RenderOutputType::Normal:
                input = gpuInitializationPassNode->NormalTex->Tex;
                break;
            case RenderOutputType::Depth:
                input = gpuInitializationPassNode->DepthTex->Tex;
                break;
            case RenderOutputType::Velocity:
                if(inputs.View.RequiresVelocityWrites())
                    input = gpuInitializationPassNode->VelocityTex->Tex;
                else
                    input = gpuInitializationPassNode->SceneTex->Tex;
                break;
            case RenderOutputType::Emissive:
                input = gpuInitializationPassNode->EmissiveTex->Tex;
                break;
            case RenderOutputType::SSAO:
                if (viewProps.RunPostProcessing)
                    input = SSAONode->Output;
                else
                    input = gpuInitializationPassNode->SceneTex->Tex;
                break;
            default:
                input = postProcessNode->GetLastOutput();
                break;
            }
        }
        else
        {
            input = gpuInitializationPassNode->SceneTex->Tex;
        }

        SPtr<RenderTarget> target = viewProps.Target.Target;

        inputs.CurrRenderAPI.SetRenderTarget(target);
        inputs.CurrRenderAPI.SetViewport(viewProps.Target.NrmViewRect);

        // If no post process is active, the only available texture is orwardPassNode->SceneTex->Tex;
        if (!input)
            input = gpuInitializationPassNode->SceneTex->Tex;

        gRendererUtility().Blit(input, Rect2I::EMPTY, viewProps.FlipView, false);

        Camera* sceneCamera = inputs.View.GetSceneCamera();
        if (sceneCamera != nullptr)
            inputs.View.NotifyCompositorTargetChanged(target);

        if (inputs.View.GetSceneCamera()->IsMain() && GuiAPI::Instance().IsGuiInitialized())
            GuiAPI::Instance().EndFrame();

        inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Final, postProcessNode->GetLastOutput());
        inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Color, gpuInitializationPassNode->SceneTex->Tex);
        inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Normal, gpuInitializationPassNode->NormalTex->Tex);
        inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Depth, gpuInitializationPassNode->DepthTex->Tex);

        if (gpuInitializationPassNode->EmissiveTex)
            inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Emissive, gpuInitializationPassNode->EmissiveTex->Tex);
        if (gpuInitializationPassNode->VelocityTex)
            inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::Velocity, gpuInitializationPassNode->VelocityTex->Tex);
        if (SSAONode)
            inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::SSAO, SSAONode->Output);

        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeFinalResolve::Clear()
    { }

    Vector<String> RCNodeFinalResolve::GetDependencies(const RendererView& view)
    {
        const RendererViewProperties& viewProps = view.GetProperties();

        Vector<String> deps;
        if (viewProps.RunPostProcessing && viewProps.Target.NumSamples == 1)
        {
            deps.push_back(RCNodeGpuInitializationPass::GetNodeId());
            deps.push_back(RCNodeForwardPass::GetNodeId());
            deps.push_back(RCNodeSkybox::GetNodeId());
            deps.push_back(RCNodeForwardTransparentPass::GetNodeId());
            deps.push_back(RCNodePostProcess::GetNodeId());
            deps.push_back(RCNodeFXAA::GetNodeId());
            deps.push_back(RCNodeTemporalAA::GetNodeId());
            deps.push_back(RCNodeSSAO::GetNodeId());
            deps.push_back(RCNodeMotionBlur::GetNodeId());
            deps.push_back(RCNodeBloom::GetNodeId());
            deps.push_back(RCNodeGaussianDOF::GetNodeId());
            deps.push_back(RCNodeTonemapping::GetNodeId());
        }
        else
        {
            deps.push_back(RCNodeGpuInitializationPass::GetNodeId());
            deps.push_back(RCNodeForwardPass::GetNodeId());
            deps.push_back(RCNodeSkybox::GetNodeId());
            deps.push_back(RCNodeForwardTransparentPass::GetNodeId());
            deps.push_back(RCNodePostProcess::GetNodeId());
        }

        return deps;
    }
}
