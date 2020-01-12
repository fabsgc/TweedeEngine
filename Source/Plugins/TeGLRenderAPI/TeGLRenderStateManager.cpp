#include "TeGLRenderStateManager.h"
#include "TeGLDepthStencilState.h"
#include "TeGLRasterizerState.h"
#include "TeGLSamplerState.h"
#include "TeGLBlendState.h"

namespace te
{
    SPtr<SamplerState> GLRenderStateManager::CreateSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
    {
        SPtr<SamplerState> ret = te_core_ptr<SamplerState>(new (te_allocate<GLSamplerState>()) GLSamplerState(desc));
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<BlendState> GLRenderStateManager::CreateBlendStateInternal(const BLEND_STATE_DESC& desc) const
    {
        SPtr<BlendState> ret = te_core_ptr<BlendState>(new (te_allocate<GLBlendState>()) GLBlendState(desc));
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<RasterizerState> GLRenderStateManager::CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
    {
        SPtr<RasterizerState> ret = te_core_ptr<RasterizerState>(new (te_allocate<GLRasterizerState>()) GLRasterizerState(desc));
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<DepthStencilState> GLRenderStateManager::CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
    {
        SPtr<DepthStencilState> ret = te_core_ptr<DepthStencilState>(new (te_allocate<GLDepthStencilState>()) GLDepthStencilState(desc)); // TODO
        ret->SetThisPtr(ret);

        return ret;
    }
}
