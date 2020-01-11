#include "RenderAPI/TeRenderStateManager.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeRasterizerState.h"

namespace te
{
    SPtr<DepthStencilState> RenderStateManager::CreateDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> state = _createDepthStencilState(desc);
		state->Initialize();

		return state;
	}

	SPtr<RasterizerState> RenderStateManager::CreateRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> state = _createRasterizerState(desc);
		state->Initialize();

		return state;
	}

    SPtr<GraphicsPipelineState> RenderStateManager::CreateGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const
	{
		SPtr<GraphicsPipelineState> state = _createGraphicsPipelineState(desc);
		state->Initialize();

		return state;
	}

    SPtr<GraphicsPipelineState> RenderStateManager::_createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const
	{
		SPtr<GraphicsPipelineState> pipelineState =
			te_core_ptr<GraphicsPipelineState>(new (te_allocate<GraphicsPipelineState>()) GraphicsPipelineState(desc));
		pipelineState->SetThisPtr(pipelineState);

		return pipelineState;
	}

    SPtr<DepthStencilState> RenderStateManager::_createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> state = CreateDepthStencilStateInternal(desc);
		return state;
	}

	SPtr<RasterizerState> RenderStateManager::_createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> state = CreateRasterizerStateInternal(desc);
		return state;
	}

    const SPtr<RasterizerState>& RenderStateManager::GetDefaultRasterizerState() const
	{
		if(_defaultRasterizerState == nullptr)
        {
			_defaultRasterizerState = CreateRasterizerState(RASTERIZER_STATE_DESC());
        }

		return _defaultRasterizerState;
	}

	const SPtr<DepthStencilState>& RenderStateManager::GetDefaultDepthStencilState() const
	{
		if(_defaultDepthStencilState == nullptr)
        {
			_defaultDepthStencilState = CreateDepthStencilState(DEPTH_STENCIL_STATE_DESC());
        }

		return _defaultDepthStencilState;
	}

    SPtr<DepthStencilState> RenderStateManager::CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> state = te_core_ptr<DepthStencilState>(new (te_allocate<DepthStencilState>()) DepthStencilState(desc));
		state->SetThisPtr(state);

		return state;
	}

	SPtr<RasterizerState> RenderStateManager::CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> state = te_core_ptr<RasterizerState>(new (te_allocate<RasterizerState>()) RasterizerState(desc));
		state->SetThisPtr(state);

		return state;
	}

    void RenderStateManager::OnShutDown()
	{
		_defaultDepthStencilState = nullptr;
		_defaultRasterizerState = nullptr;
	}
}
