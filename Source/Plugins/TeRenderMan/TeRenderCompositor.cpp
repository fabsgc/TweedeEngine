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
#include "PostProcessing/TeSSAODownsampleMat.h"
#include "PostProcessing/TeSSAOMat.h"
#include "PostProcessing/TeSSAOBlurMat.h"
#include "Renderer/Materials/TeGaussianBlurMat.h"

namespace te
{
    UnorderedMap<String, RenderCompositor::NodeType*> RenderCompositor::_nodeTypes;

    struct ZPrepassElem
    {
        const SPtr<Mesh>* MeshElem = nullptr; // Can be a Mesh of a ZPrepassMesh
        SubMesh* SubMeshElem = nullptr;
        const SPtr<GpuParams>* GpuParamsElem = nullptr;
        UINT32 InstanceCount = 0;

        ~ZPrepassElem()
        { }
    };

    IMPLEMENT_GLOBAL_POOL(ZPrepassElem, 32)

    /** Renders all elements for the Z Prepass. */
    UINT32 RenderQueueElementsForZPrepass(const Vector<RenderQueueElement>& elements, const RendererView& view, const SceneInfo& scene, const RendererViewGroup& viewGroup)
    {
        UINT32 drawCallsCounter = 0;
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<Material> lastMaterial = nullptr;

        static const Vector<String> InstancedBuffer = { "PerInstanceBuffer" };
        static const Vector<String> CameraBuffer = { "PerCameraBuffer" };
        static const Vector<String> ObjectBuffer = { "PerObjectBuffer" };

        Vector<ZPrepassElem*> zPrepassMeshElements;
        Vector<ZPrepassElem*> zMeshElements;
        UnorderedSet<Mesh*> zPrepassMeshTreated;

        for (auto& entry : elements)
        {
            if (!entry.RenderElem->UseForZPrepass)
                continue;

            SPtr<Mesh> zPrepassMesh = entry.RenderElem->ZPrepassMeshElem;

            if (zPrepassMesh && entry.RenderElem->InstanceCount == 0)
            {
                if (zPrepassMeshTreated.emplace(zPrepassMesh.get()).second)
                {
                    MeshProperties& properties = zPrepassMesh->GetProperties();

                    for (UINT32 i = 0; i < properties.GetNumSubMeshes(); i++)
                    {
                        ZPrepassElem* zPrepassElem = te_pool_new<ZPrepassElem>();
                        zPrepassElem->MeshElem = &entry.RenderElem->ZPrepassMeshElem;
                        zPrepassElem->SubMeshElem = properties.GetSubMeshPtr(i);
                        zPrepassElem->InstanceCount = 0;
                        zPrepassElem->GpuParamsElem = &entry.RenderElem->GpuParamsElem[entry.PassIdx];
                        zPrepassMeshElements.push_back(zPrepassElem);
                    }
                }
            }
            else
            {
                ZPrepassElem* zPrepassElem = te_pool_new<ZPrepassElem>();
                zPrepassElem->MeshElem = &entry.RenderElem->MeshElem;
                zPrepassElem->SubMeshElem = entry.RenderElem->SubMeshElem;
                zPrepassElem->InstanceCount = entry.RenderElem->InstanceCount;
                zPrepassElem->GpuParamsElem = &entry.RenderElem->GpuParamsElem[entry.PassIdx];
                zMeshElements.push_back(zPrepassElem);
            }
        }

        auto DrawZPrepassElem = [&](SPtr<Pass>& pass, ZPrepassElem* zPrepassElem)
        {
            rapi.PushMarker("[DRAW] Renderable", Color(0.7f, 0.8f, 0.2f));
            rapi.SetGraphicsPipeline(pass->GetGraphicsPipelineState());
            rapi.SetStencilRef(pass->GetStencilRefValue());

            (*zPrepassElem->GpuParamsElem)->SetParamBlockBuffer("PerCameraBuffer", view.GetPerViewBuffer());

            if (zPrepassElem->InstanceCount > 0)
            {
                rapi.SetGpuParams((*zPrepassElem->GpuParamsElem), GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_ALL);
            }
            else
            {
                rapi.SetGpuParams((*zPrepassElem->GpuParamsElem), GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_LISTED, CameraBuffer);
                rapi.SetGpuParams((*zPrepassElem->GpuParamsElem), GPU_BIND_PARAM_BLOCK, GPU_BIND_PARAM_BLOCK_LISTED, ObjectBuffer);
            }

            gRendererUtility().Draw((*zPrepassElem->MeshElem), *zPrepassElem->SubMeshElem, zPrepassElem->InstanceCount);
            rapi.PopMarker();

            drawCallsCounter++;
        };

        if(zPrepassMeshElements.size() > 0)
        {
            HShader shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ZPrepass);
            const auto& techniques = shader->GetTechniques();

            if (techniques.size() == 0)
                return 0;

            SPtr<Technique> technique = techniques[0];
            if (technique->GetNumPasses() == 0)
                return 0;

            SPtr<Pass> pass = technique->GetPass(0);

            for (auto& zPrepassElem : zPrepassMeshElements)
            {
                DrawZPrepassElem(pass, zPrepassElem);
                te_pool_delete<ZPrepassElem>(static_cast<ZPrepassElem*>(zPrepassElem));
            }
        }

        if(zMeshElements.size() > 0)
        {
            HShader shader = gBuiltinResources().GetBuiltinShader(BuiltinShader::ZPrepass);
            const auto& techniques = shader->GetTechniques();

            if (techniques.size() == 0)
                return 0;

            SPtr<Technique> technique = techniques[0];
            if (technique->GetNumPasses() == 0)
                return 0;

            SPtr<Pass> pass = technique->GetPass(0);

            for (auto& zPrepassElem : zMeshElements)
            {
                DrawZPrepassElem(pass, zPrepassElem);
                te_pool_delete<ZPrepassElem>(static_cast<ZPrepassElem*>(zPrepassElem));
            }
        }

        return drawCallsCounter;
    }

    /** Renders all elements in a render queue. */
    UINT32 RenderQueueElements(const Vector<RenderQueueElement>& elements, const RendererView& view, const SceneInfo& scene, const RendererViewGroup& viewGroup)
    {
        UINT32 drawCallsCounter = 0;
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
                gRendererUtility().SetPass(entry.RenderElem->MaterialElem, entry.PassIdx, entry.TechniqueIdx);

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
            drawCallsCounter++;
        }

        return drawCallsCounter;
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
            VelocityTex = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RG16S, width, height, TU_RENDERTARGET,
                numSamples, false));
        }

        DepthTex = gGpuResourcePool().Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_D32_S8X24, width, height, TU_DEPTHSTENCIL,
            numSamples, false));

        bool rebuildRT = false;
        if (RenderTargetTex)
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
        {
            rebuildRT = true;
        }

        bool rebuildZPrepassRT = false;
        if (RenderTargetZPrepassTex)
            rebuildZPrepassRT = RenderTargetZPrepassTex->GetDepthStencilTexture() != DepthTex->Tex;
        else
            rebuildZPrepassRT = true;

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

        if (RenderTargetZPrepassTex == nullptr || rebuildZPrepassRT)
        {
            RENDER_TEXTURE_DESC gbufferDesc;
            gbufferDesc.DepthStencilSurface.Tex = DepthTex->Tex;
            gbufferDesc.DepthStencilSurface.Face = 0;
            gbufferDesc.DepthStencilSurface.MipLevel = 0;

            RenderTargetZPrepassTex = RenderTexture::Create(gbufferDesc);
        }
    }

    void RCNodeGpuInitializationPass::Clear()
    {
        SceneTex = nullptr;
        NormalTex = nullptr;
        EmissiveTex = nullptr;
        VelocityTex = nullptr;
        DepthTex = nullptr;
        DrawCallsCounter = 0;
    }

    Vector<String> RCNodeGpuInitializationPass::GetDependencies(const RendererView& view)
    {
        return { };
    }

    void RCNodeZPrePass::Render(const RenderCompositorNodeInputs& inputs)
    {
        RenderQueue* opaqueElements = inputs.View.GetOpaqueQueue().get();
        const Vector<RenderQueueElement> elements = opaqueElements->GetSortedElements();
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        if (elements.size() == 0)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Z Pre Pass", Color(0.7f, 0.41f, 0.36f));
        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetZPrepassTex);
        inputs.CurrRenderAPI.ClearViewport(FBT_DEPTH | FBT_STENCIL, Color::Black);

        if (inputs.View.GetRenderSettings().UseZPrepass && elements.size() > 0)
        {
            gpuInitializationPassNode->DrawCallsCounter = RenderQueueElementsForZPrepass(elements, inputs.View, inputs.Scene, inputs.ViewGroup);
        }

        // Make sure that any compute shaders are able to read g-buffer by unbinding it
        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeZPrePass::Clear()
    { }

    Vector<String> RCNodeZPrePass::GetDependencies(const RendererView& view)
    {
        return { RCNodeGpuInitializationPass::GetNodeId() };
    }

    // ############# FORWARD PASS

    void RCNodeForwardPass::Render(const RenderCompositorNodeInputs& inputs)
    {
        RenderQueue* opaqueElements = inputs.View.GetOpaqueQueue().get();
        const Vector<RenderQueueElement> elements = opaqueElements->GetSortedElements();
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        if (elements.size() == 0)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Forward Pass", Color(0.8f, 0.6f, 0.8f));
        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex);
        inputs.CurrRenderAPI.ClearViewport(FBT_COLOR, Color::Black);

        // Render all opaque elements     
        gpuInitializationPassNode->DrawCallsCounter = RenderQueueElements(elements, inputs.View, inputs.Scene, inputs.ViewGroup);

        // Make sure that any compute shaders are able to read g-buffer by unbinding it
        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeForwardPass::Clear()
    { }

    Vector<String> RCNodeForwardPass::GetDependencies(const RendererView& view)
    {
        return { 
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodeZPrePass::GetNodeId()
        };
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

        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex, FBT_DEPTH | FBT_STENCIL);

        SPtr<Mesh> mesh = gRendererUtility().GetSkyBoxMesh();
        gRendererUtility().Draw(mesh, mesh->GetProperties().GetSubMesh(0));

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
        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);

        if (elements.size() == 0)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Forward Transparent Pass", Color(0.6f, 0.6f, 0.9f));
        inputs.CurrRenderAPI.SetRenderTarget(gpuInitializationPassNode->RenderTargetTex, FBT_DEPTH);

        gpuInitializationPassNode->DrawCallsCounter = RenderQueueElements(elements, inputs.View, inputs.Scene, inputs.ViewGroup);

        // Make sure that any compute shaders are able to read g-buffer by unbinding it
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

    void RCNodeHalfSceneTex::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Half Scene Tex", Color(0.74f, 0.21f, 0.32f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
            return;

        SPtr<Texture> input = gpuInitializationPassNode->SceneTex->Tex;
        TextureDownsampleMat* downsampleMat = TextureDownsampleMat::Get();
        const TextureProperties& rtProps = input->GetProperties();

        SceneTex = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                rtProps.GetFormat(), 
                rtProps.GetWidth() / 2, 
                rtProps.GetHeight() / 2, 
                TU_RENDERTARGET)
        );

        EmissiveTex = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                rtProps.GetFormat(),
                rtProps.GetWidth() / 2,
                rtProps.GetHeight() / 2,
                TU_RENDERTARGET)
        );

        downsampleMat->Execute(gpuInitializationPassNode->SceneTex->Tex, 0, SceneTex->RenderTex);
        downsampleMat->Execute(gpuInitializationPassNode->EmissiveTex->Tex, 0, EmissiveTex->RenderTex);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeHalfSceneTex::Clear()
    {
        SceneTex = nullptr;
        EmissiveTex = nullptr;
    }

    Vector<String> RCNodeHalfSceneTex::GetDependencies(const RendererView& view)
    {
        return { 
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };
    }

    constexpr UINT32 RCNodeSceneTexDownsamples::MAX_NUM_DOWNSAMPLES;

    void RCNodeSceneTexDownsamples::Render(const RenderCompositorNodeInputs& inputs)
    {
        inputs.CurrRenderAPI.PushMarker("[DRAW] Scene Tex Down Samples", Color(0.44f, 0.71f, 0.52f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
            return;

        auto DownSample = [this](
            SPtr<PooledRenderTexture> renderTex,
            SPtr<PooledRenderTexture>* output)
        {
            const TextureProperties& props = renderTex->Tex->GetProperties();
            const UINT32 totalDownsampleLevels = PixelUtil::GetMaxMipmaps(
                props.GetWidth(),
                props.GetHeight(),
                1) + 1;

            AvailableDownsamples = Math::Min(MAX_NUM_DOWNSAMPLES, totalDownsampleLevels);
            output[0] = renderTex;

            TextureDownsampleMat* downsampleMat = TextureDownsampleMat::Get();
            for (UINT32 i = 1; i < AvailableDownsamples; i++)
            {
                const TextureProperties& rtProps = output[i - 1]->Tex->GetProperties();

                output[i] = gGpuResourcePool().Get(
                    POOLED_RENDER_TEXTURE_DESC::Create2D(
                        rtProps.GetFormat(),
                        rtProps.GetWidth() / 2,
                        rtProps.GetHeight() / 2,
                        TU_RENDERTARGET
                    )
                );

                downsampleMat->Execute(output[i - 1]->Tex, 0, output[i]->RenderTex);
            }
        };

        RCNodeHalfSceneTex* halfSceneTexNode = static_cast<RCNodeHalfSceneTex*>(inputs.InputNodes[2]);
        DownSample(halfSceneTexNode->SceneTex, &SceneTex[0]);
        DownSample(halfSceneTexNode->EmissiveTex, &EmissiveTex[0]);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeSceneTexDownsamples::Clear()
    { 
        for (UINT32 i = 0; i < MAX_NUM_DOWNSAMPLES; i++)
        {
            SceneTex[i] = nullptr;
            EmissiveTex[i] = nullptr;
        }
    }

    Vector<String> RCNodeSceneTexDownsamples::GetDependencies(const RendererView& view)
    {
        return 
        {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId(),
            RCNodeHalfSceneTex::GetNodeId()
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
            RCNodeGpuInitializationPass::GetNodeId()
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
        RCNodeSSAO* ssaoNode = static_cast<RCNodeSSAO*>(inputs.InputNodes[2]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        if (!ppLastFrame)
            ppLastFrame = gpuInitializationPassNode->SceneTex->Tex;

        auto& texProps = ppLastFrame->GetProperties();

        ToneMappingMat* toneMapping = ToneMappingMat::GetVariation(texProps.GetNumSamples(), !settings.Tonemapping.Enabled);
        TE_ASSERT_ERROR(toneMapping != nullptr, "Failed to retrieve variation of ToneMappingMat material");

        toneMapping->Execute((ssaoNode->Output) ? ssaoNode->Output->Tex : Texture::WHITE, ppLastFrame, ppOutput, texProps.GetNumSamples(),
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
            RCNodePostProcess::GetNodeId(),
            RCNodeSSAO::GetNodeId()
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

        if (inputs.View.GetSceneCamera()->GetProjectionType() == ProjectionType::PT_ORTHOGRAPHIC)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Motion Blur", Color(0.1f, 0.9f, 0.6f));

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
            return;

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

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
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
    { 
        // TODO Gaussian DOF
    }

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
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
            return;

        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[1]);

        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;
        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        FXAAMat* fxaa = FXAAMat::Get();

        if(ppLastFrame)
            fxaa->Execute(ppLastFrame, ppOutput);
        else
            fxaa->Execute(gpuInitializationPassNode->SceneTex->Tex, ppOutput);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
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

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
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

        if (inputs.View.GetSceneCamera()->GetProjectionType() == ProjectionType::PT_ORTHOGRAPHIC)
        {
            Output = nullptr;
            return;
        }

        const AmbientOcclusionSettings& settings = inputs.View.GetRenderSettings().AmbientOcclusion;
        if (!settings.Enabled)
        {
            Output = nullptr;
            return;
        }

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
        {
            Output = nullptr;
            return;
        }

        inputs.CurrRenderAPI.PushMarker("[DRAW] SSAO", Color(0.25f, 0.35f, 0.95f));

        GpuResourcePool& resPool = gGpuResourcePool();
        const RendererViewProperties& viewProps = inputs.View.GetProperties();
        RCNodeResolvedSceneDepth* resolvedDepthNode = static_cast<RCNodeResolvedSceneDepth*>(inputs.InputNodes[2]);

        SPtr<Texture> sceneNormals = gpuInitializationPassNode->NormalTex->Tex;
        SPtr<Texture> sceneDepth = resolvedDepthNode->Output->Tex;

        const TextureProperties& normalsProps = sceneNormals->GetProperties();
        SPtr<PooledRenderTexture> resolvedNormals;

        // Multi sampled sceneDepth is already resolved, we need to do the same with sceneNormal
        if (sceneNormals->GetProperties().GetNumSamples() > 1)
        {
            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(normalsProps.GetFormat(),
                normalsProps.GetWidth(), normalsProps.GetHeight(), TU_RENDERTARGET);
            resolvedNormals = resPool.Get(desc);

            inputs.CurrRenderAPI.SetRenderTarget(resolvedNormals->RenderTex);
            gRendererUtility().Blit(sceneNormals);

            sceneNormals = resolvedNormals->Tex;
        }

        // Multiple downsampled AO levels are used to minimize cache trashing. Downsampled AO targets use larger radius,
        // whose contents are then blended with the higher level.
        AmbientOcclusionQuality quality = settings.Quality;
        UINT32 numDownsampleLevels = 0;
        if (quality == AmbientOcclusionQuality::Medium)
            numDownsampleLevels = 1;
        else if (quality > AmbientOcclusionQuality::Medium)
            numDownsampleLevels = 2;

        SSAODownsampleMat* downsample = SSAODownsampleMat::Get();

        SPtr<PooledRenderTexture> setupTex0;
        if (numDownsampleLevels > 0)
        {
            Vector2I downsampledSize(
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.width, 2)),
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.height, 2))
            );

            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA16F, downsampledSize.x,
                downsampledSize.y, TU_RENDERTARGET);
            setupTex0 = resPool.Get(desc);

            downsample->Execute(inputs.View, sceneDepth, sceneNormals, setupTex0->RenderTex, DEPTH_RANGE);
        }

        /*SPtr<PooledRenderTexture> setupTex1;
        if(numDownsampleLevels > 1)
        {
            Vector2I downsampledSize(
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.width, 4)),
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.height, 4))
            );

            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(PF_RGBA16F, downsampledSize.x,
                downsampledSize.y, TU_RENDERTARGET);
            setupTex1 = resPool.Get(desc);

            downsample->Execute(inputs.View, sceneDepth, sceneNormals, setupTex1->RenderTex, DEPTH_RANGE);
        }*/

        SSAOTextureInputs textures;
        textures.SceneDepth = sceneDepth;
        textures.SceneNormals = sceneNormals;
        textures.RandomRotations = RendererTextures::SSAORandomization4x4;

        SPtr<PooledRenderTexture> downAOTex1;
        if (numDownsampleLevels > 1)
        {
            //textures.AOSetup = setupTex1->Tex;
            textures.AOSetup = setupTex0->Tex;

            Vector2I downsampledSize(
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.width, 4)),
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.height, 4))
            );

            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(PF_R8, downsampledSize.x,
                downsampledSize.y, TU_RENDERTARGET);
            downAOTex1 = resPool.Get(desc);

            SSAOMat* ssaoMat = SSAOMat::Get();
            ssaoMat->Execute(inputs.View, textures, downAOTex1->RenderTex, settings, false, false, (UINT32)quality);

            //setupTex1 = nullptr;
            setupTex0 = nullptr;
        }

        SPtr<PooledRenderTexture> downAOTex0;
        /*if (numDownsampleLevels > 0)
        {
            textures.AOSetup = setupTex0->Tex;

            if (downAOTex1)
                textures.AODownsampled = downAOTex1->Tex;

            Vector2I downsampledSize(
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.width, 2)),
                std::max(1, Math::DivideAndRoundUp((INT32)viewProps.Target.ViewRect.height, 2))
            );

            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(PF_R8, downsampledSize.x,
                downsampledSize.y, TU_RENDERTARGET);
            downAOTex0 = resPool.Get(desc);

            bool upSample = numDownsampleLevels > 1;
            SSAOMat* ssaoMat = SSAOMat::Get();
            ssaoMat->Execute(inputs.View, textures, downAOTex0->RenderTex, settings, upSample, false, (UINT32)quality);

            if (upSample)
                downAOTex1 = nullptr;
        }*/

        UINT32 width = viewProps.Target.ViewRect.width;
        UINT32 height = viewProps.Target.ViewRect.height;
        Output = resPool.Get(POOLED_RENDER_TEXTURE_DESC::Create2D(PF_R8, width, height, TU_RENDERTARGET));

        {
            if (setupTex0)
                textures.AOSetup = setupTex0->Tex;

            if (downAOTex0)
                textures.AODownsampled = downAOTex0->Tex;

            //bool upSample = numDownsampleLevels > 0;
            bool upSample = false;
            SSAOMat* ssaoMat = SSAOMat::Get();
            ssaoMat->Execute(inputs.View, textures, Output->RenderTex, settings, upSample, true, (UINT32)quality);
        }

        resolvedNormals = nullptr;

        if (numDownsampleLevels > 0)
        {
            setupTex0 = nullptr;
            downAOTex0 = nullptr;
        }

        // Blur the output
        // Note: If I implement temporal AA then this can probably be avoided. I can instead jitter the sample offsets
        // each frame, and averaging them out should yield blurred AO.
        if (quality > AmbientOcclusionQuality::Low) // On level 0 we don't blur at all, on level 1 we use the ad-hoc blur in shader
        {
            const RenderTargetProperties& rtProps = Output->RenderTex->GetProperties();

            POOLED_RENDER_TEXTURE_DESC desc = POOLED_RENDER_TEXTURE_DESC::Create2D(PF_R8, rtProps.Width,
                rtProps.Height, TU_RENDERTARGET);
            SPtr<PooledRenderTexture> blurIntermediateTex = resPool.Get(desc);

            SSAOBlurMat* ssaoBlurMat = SSAOBlurMat::Get();

            ssaoBlurMat->Execute(inputs.View, Output->Tex, sceneDepth, blurIntermediateTex->RenderTex, DEPTH_RANGE, true);
            ssaoBlurMat->Execute(inputs.View, blurIntermediateTex->Tex, sceneDepth, Output->RenderTex, DEPTH_RANGE, false);
        }

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeSSAO::Clear()
    {
        Output = nullptr;
    }

    Vector<String> RCNodeSSAO::GetDependencies(const RendererView& view)
    {
        Vector<String> deps = {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };

        if(view.GetRenderSettings().AmbientOcclusion.Enabled)
            deps.push_back(RCNodeResolvedSceneDepth::GetNodeId());

        return deps;
    }

    // ############# BLOOM

    void RCNodeBloom::Render(const RenderCompositorNodeInputs& inputs)
    {
        const RendererViewProperties& viewProps = inputs.View.GetProperties();
        const RenderSettings& settings = inputs.View.GetRenderSettings();
        if (!settings.Bloom.Enabled)
            return;

        RCNodeGpuInitializationPass* gpuInitializationPassNode = static_cast<RCNodeGpuInitializationPass*>(inputs.InputNodes[0]);
        if (gpuInitializationPassNode->DrawCallsCounter == 0)
            return;

        inputs.CurrRenderAPI.PushMarker("[DRAW] Bloom", Color(0.85f, 0.55f, 0.15f));

        RCNodePostProcess* postProcessNode = static_cast<RCNodePostProcess*>(inputs.InputNodes[1]);
        RCNodeSceneTexDownsamples* sceneColorDownSampleNode = static_cast<RCNodeSceneTexDownsamples*>(inputs.InputNodes[2]);

        constexpr UINT32 NUM_STEPS_PER_QUALITY[] = { 1, 1, 2, 3 };
        constexpr UINT32 OUTPUT_DOWN_SAMPLE_LEVEL_PER_QUALITY[] = { 4, 3, 2, 1 };

        // ### First, we get emissive texture from forward pass, use it as input for our GaussianBlur material
        // ### and create a new tex representing the blured result

        // We can reduce blur texture size according to bloom quality
        const UINT32 quality = Math::Clamp((UINT32)settings.Bloom.Quality, 0U, 3U);
        const UINT32 downSampleLevel = OUTPUT_DOWN_SAMPLE_LEVEL_PER_QUALITY[quality];

        const UINT32 availableDownsamples = sceneColorDownSampleNode->AvailableDownsamples;
        const UINT32 numDownsamples = Math::Min(availableDownsamples, downSampleLevel);
        assert(numDownsamples >= 1);

        SPtr<PooledRenderTexture> emissiveTex = sceneColorDownSampleNode->EmissiveTex[numDownsamples - 1];

        const TextureProperties& inputProps = emissiveTex->Tex->GetProperties();
        SPtr<PooledRenderTexture> tmpBlurOutput = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                inputProps.GetFormat(),
                inputProps.GetWidth(),
                inputProps.GetHeight(),
                TU_RENDERTARGET,
                viewProps.Target.NumSamples
            )
        );

        SPtr<PooledRenderTexture> blurOutput = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(
                inputProps.GetFormat(),
                inputProps.GetWidth(),
                inputProps.GetHeight(),
                TU_RENDERTARGET,
                viewProps.Target.NumSamples
            )
        );

        GaussianBlurMat* gaussianBlur = GaussianBlurMat::GetVariation(emissiveTex->Tex->GetProperties().GetNumSamples());

        gaussianBlur->Execute(emissiveTex->Tex, tmpBlurOutput->RenderTex, settings.Bloom.FilterSize, settings.Bloom.MaxBlurSamples,
            settings.Bloom.Tint, emissiveTex->Tex->GetProperties().GetNumSamples());
        gaussianBlur->Execute(tmpBlurOutput->Tex, blurOutput->RenderTex, settings.Bloom.FilterSize, settings.Bloom.MaxBlurSamples,
            settings.Bloom.Tint, emissiveTex->Tex->GetProperties().GetNumSamples());
        gaussianBlur->Execute(blurOutput->Tex, tmpBlurOutput->RenderTex, settings.Bloom.FilterSize, settings.Bloom.MaxBlurSamples,
            settings.Bloom.Tint, emissiveTex->Tex->GetProperties().GetNumSamples());

        // ### Once we have our blured texture, we call our bloom material which will add this blured texture to the 
        // ### output final texture
        SPtr<RenderTexture> ppOutput;
        SPtr<Texture> ppLastFrame;

        postProcessNode->GetAndSwitch(inputs.View, ppOutput, ppLastFrame);

        if (!ppLastFrame)
            ppLastFrame = gpuInitializationPassNode->SceneTex->Tex;

        auto& texProps = ppLastFrame->GetProperties();

        BloomMat* bloom = BloomMat::GetVariation(texProps.GetNumSamples());
        bloom->Execute(ppLastFrame, ppOutput, tmpBlurOutput->Tex,
            settings.Bloom.Intensity);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
        inputs.CurrRenderAPI.PopMarker();
    }

    void RCNodeBloom::Clear()
    { }

    Vector<String> RCNodeBloom::GetDependencies(const RendererView& view)
    {
        Vector<String> deps = {
            RCNodeGpuInitializationPass::GetNodeId(),
            RCNodePostProcess::GetNodeId()
        };

        if(view.GetRenderSettings().Bloom.Enabled)
            deps.push_back(RCNodeSceneTexDownsamples::GetNodeId());

        return deps;
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
                    input = SSAONode->Output->Tex;
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
        if (SSAONode && SSAONode->Output)
            inputs.CurrRenderer.SetLastRenderTexture(RenderOutputType::SSAO, SSAONode->Output->Tex);

        inputs.CurrRenderAPI.SetRenderTarget(nullptr);
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
