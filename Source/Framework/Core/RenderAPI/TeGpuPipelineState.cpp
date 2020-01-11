#include "TeGpuPipelineState.h"

namespace te
{
    GraphicsPipelineState::GraphicsPipelineState(const PIPELINE_STATE_DESC& desc)
    {
    }

    void GraphicsPipelineState::Initialize()
	{
        CoreObject::Initialize();
    }

    SPtr<GraphicsPipelineState> GraphicsPipelineState::Create(const PIPELINE_STATE_DESC& desc)
    {
        GraphicsPipelineState* graphicsPipelineState = new (te_allocate<GraphicsPipelineState>()) GraphicsPipelineState(desc);
        SPtr<GraphicsPipelineState> handlerPtr = te_shared_ptr<GraphicsPipelineState>(graphicsPipelineState);
        handlerPtr->SetThisPtr(handlerPtr);

        return handlerPtr;
    }
}
