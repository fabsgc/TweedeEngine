#include "TeRenderCompositor.h"
#include "Renderer/TeCamera.h"
#include "Mesh/TeMesh.h"
#include "TeRendererView.h"
#include "TeRenderManOptions.h"
#include "TeRendererScene.h"
#include "TeRenderMan.h"

namespace te
{
    UnorderedMap<String, RenderCompositor::NodeType*> RenderCompositor::_nodeTypes;

    void SetPass(const SPtr<Material>& material, UINT32 passIdx, UINT32 techniqueIdx)
    {
        RenderAPI& rapi = RenderAPI::Instance();

        SPtr<Pass> pass = material->GetPass(passIdx, techniqueIdx);
        rapi.SetGraphicsPipeline(pass->GetGraphicsPipelineState());
        rapi.SetStencilRef(pass->GetStencilRefValue());
    }

    void SetPassParams(const SPtr<GpuParams> gpuParams)
    {
        if (gpuParams == nullptr)
            return;

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetGpuParams(gpuParams);
    }

    /** Renders all elements in a render queue. */
    void RenderQueueElements(const Vector<RenderQueueElement>& elements)
    {
        for(auto& entry : elements)
        {
            if(entry.ApplyPass)
                SetPass(entry.RenderElem->MaterialElem, entry.TechniqueIdx, entry.PassIdx);

            SetPassParams(entry.RenderElem->GpuParamsElem[entry.PassIdx]);
            entry.RenderElem->Draw();
        }
    }

    RenderCompositor::~RenderCompositor()
    {
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
                TE_DEBUG("Cannot find render compositor node of type \"{" + String(nodeId.c_str()) + "}\".", __FILE__, __LINE__);
                return false;
            }

            NodeType* nodeType = iterFind->second;

            // Register current node
            auto iterFind2 = processedNodes.find(nodeId);

            // New node
            if (iterFind2 == processedNodes.end())
            {
                // Mark it as invalid for now
                processedNodes[nodeId] = -1;
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
                nodeInfo.Node = nodeType->Create();
                nodeInfo.Type = nodeType;
                nodeInfo.LastUseIdx = -1;

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
                        "depends on node \"{" + String(iterFind->first.c_str()) + "}\" which is not available at this stage.", __FILE__, __LINE__);
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

        Vector<const NodeInfo*> activeNodes;

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

        if (!_nodeInfos.empty())
            _nodeInfos.back().Node->Clear();
    }

    void RenderCompositor::Clear()
    {
        for (auto& entry : _nodeInfos)
            te_delete(entry.Node);

        _nodeInfos.clear();
        _isValid = false;
    }

    void RCNodeForwardPass::Render(const RenderCompositorNodeInputs& inputs)
    { 
        const RendererViewProperties& viewProps = inputs.View.GetProperties();
        const VisibilityInfo& visibility = inputs.View.GetVisibilityInfo();
        const auto numRenderables = (UINT32)inputs.Scene.Renderables.size();
        for (UINT32 i = 0; i < numRenderables; i++)
        {
            if (!visibility.Renderables[i])
                continue;

            RendererRenderable* rendererRenderable = inputs.Scene.Renderables[i];
            rendererRenderable->UpdatePerCallBuffer(viewProps.ViewProjTransform);

            for (auto& element : inputs.Scene.Renderables[i]->Elements)
            {
                Vector<SPtr<GpuParams>>& passesGpuParams = element.GpuParamsElem;
                for (auto& gpuParams : passesGpuParams)
                {
                    // TODO call only if view has changed
                    gpuParams->SetParamBlockBuffer("PerCameraBuffer", inputs.View.GetPerViewBuffer());
                }
            }
        }

        RenderAPI& rapi = RenderAPI::Instance();

        rapi.SetRenderTarget(inputs.View.GetProperties().Target.Target);

        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
        rapi.ClearViewport(clearBuffers, inputs.View.GetProperties().Target.ClearColor);

        // Render all visible opaque elements
        RenderQueue* opaqueElements = inputs.View.GetOpaqueQueue().get();
        RenderQueueElements(opaqueElements->GetSortedElements());
    }

    void RCNodeForwardPass::Clear()
    { }

    Vector<String> RCNodeForwardPass::GetDependencies(const RendererView& view)
    {
        return { };
    }

    void RCNodeFinalResolve::Render(const RenderCompositorNodeInputs& inputs)
    { }

    void RCNodeFinalResolve::Clear()
    { }

    Vector<String> RCNodeFinalResolve::GetDependencies(const RendererView& view)
    {
        return
        {
            RCNodeForwardPass::GetNodeId()
        };
    }
}
