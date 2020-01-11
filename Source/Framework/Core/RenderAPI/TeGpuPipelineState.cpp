#include "TeGpuPipelineState.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    GraphicsPipelineState::GraphicsPipelineState(const PIPELINE_STATE_DESC& desc)
        : _data(desc)
    { }

    void GraphicsPipelineState::Initialize()
	{
        CoreObject::Initialize();
    }

    SPtr<GraphicsPipelineState> GraphicsPipelineState::Create(const PIPELINE_STATE_DESC& desc)
    {
        return RenderStateManager::Instance().CreateGraphicsPipelineState(desc);
    }
}
