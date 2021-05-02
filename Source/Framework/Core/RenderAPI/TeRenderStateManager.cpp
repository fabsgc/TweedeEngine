#include "RenderAPI/TeRenderStateManager.h"

#include "RenderAPI/TeGpuPipelineParamInfo.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeSamplerState.h"
#include "RenderAPI/TeBlendState.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RenderStateManager)

    SPtr<SamplerState> RenderStateManager::CreateSamplerState(const SAMPLER_STATE_DESC& desc) const
    {
        SPtr<SamplerState> state = _createSamplerState(desc);
        state->Initialize();

        return state;
    }

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

    SPtr<BlendState> RenderStateManager::CreateBlendState(const BLEND_STATE_DESC& desc) const
    {
        SPtr<BlendState> state = _createBlendState(desc);
        state->Initialize();

        return state;
    }

    SPtr<GraphicsPipelineState> RenderStateManager::CreateGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
        GpuDeviceFlags deviceMask) const
    {
        SPtr<GraphicsPipelineState> state = _createGraphicsPipelineState(desc, deviceMask);
        state->Initialize();

        return state;
    }

    SPtr<ComputePipelineState> RenderStateManager::CreateComputePipelineState(const SPtr<GpuProgram>& program,
        GpuDeviceFlags deviceMask) const
    {
        SPtr<ComputePipelineState> state = _createComputePipelineState(program, deviceMask);
        state->Initialize();

        return state;
    }

    SPtr<GpuPipelineParamInfo> RenderStateManager::CreatePipelineParamInfo(
        const GPU_PIPELINE_PARAMS_DESC& desc, GpuDeviceFlags deviceMask) const
    {
        SPtr<GpuPipelineParamInfo> paramInfo = _createPipelineParamInfo(desc, deviceMask);
        paramInfo->Initialize();

        return paramInfo;
    }

    SPtr<GraphicsPipelineState> RenderStateManager::_createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
        GpuDeviceFlags deviceMask) const
    {
        SPtr<GraphicsPipelineState> pipelineState =
            te_core_ptr<GraphicsPipelineState>(new (te_allocate<GraphicsPipelineState>()) GraphicsPipelineState(desc, deviceMask));
        pipelineState->SetThisPtr(pipelineState);

        return pipelineState;
    }

    SPtr<ComputePipelineState> RenderStateManager::_createComputePipelineState(const SPtr<GpuProgram>& program,
        GpuDeviceFlags deviceMask) const
    {
        SPtr<ComputePipelineState> pipelineState =
            te_core_ptr<ComputePipelineState>(new (te_allocate<ComputePipelineState>()) ComputePipelineState(program, deviceMask));
        pipelineState->SetThisPtr(pipelineState);

        return pipelineState;
    }

    SPtr<SamplerState> RenderStateManager::_createSamplerState(const SAMPLER_STATE_DESC& desc) const
    {
        SPtr<SamplerState> state = CreateSamplerStateInternal(desc);
        return state;
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

    SPtr<BlendState> RenderStateManager::_createBlendState(const BLEND_STATE_DESC& desc) const
    {
        SPtr<BlendState> state = CreateBlendStateInternal(desc);
        return state;
    }

    SPtr<GpuPipelineParamInfo> RenderStateManager::_createPipelineParamInfo(
        const GPU_PIPELINE_PARAMS_DESC& desc, GpuDeviceFlags deviceMask) const
    {
        SPtr<GpuPipelineParamInfo> paramInfo =
            te_core_ptr<GpuPipelineParamInfo>(new (te_allocate<GpuPipelineParamInfo>())
                GpuPipelineParamInfo(desc, deviceMask));

        paramInfo->SetThisPtr(paramInfo);

        return paramInfo;
    }

    const SPtr<SamplerState>& RenderStateManager::GetDefaultSamplerState() const
    {
        if (_defaultSamplerState == nullptr)
        {
            _defaultSamplerState = CreateSamplerState(SAMPLER_STATE_DESC());
        }

        return _defaultSamplerState;
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

    const SPtr<BlendState>& RenderStateManager::GetDefaultBlendState() const
    {
        if (_defaultBlendState == nullptr)
        {
            _defaultBlendState = CreateBlendState(BLEND_STATE_DESC());
        }

        return _defaultBlendState;
    }

    SPtr<SamplerState> RenderStateManager::CreateSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
    {
        SPtr<SamplerState> state = te_core_ptr<SamplerState>(new (te_allocate<SamplerState>()) SamplerState(desc));
        state->SetThisPtr(state);

        return state;
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

    SPtr<BlendState> RenderStateManager::CreateBlendStateInternal(const BLEND_STATE_DESC& desc) const
    {
        SPtr<BlendState> state = te_core_ptr<BlendState>(new (te_allocate<BlendState>()) BlendState(desc));
        state->SetThisPtr(state);

        return state;
    }

    void RenderStateManager::OnShutDown()
    {
        _defaultDepthStencilState = nullptr;
        _defaultRasterizerState = nullptr;
        _defaultSamplerState = nullptr;
        _defaultBlendState = nullptr;
    }
}
