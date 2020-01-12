#include "TeGLRenderStateManager.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeSamplerState.h"
#include "RenderAPI/TeBlendState.h"

namespace te
{
    SPtr<SamplerState> GLRenderStateManager::CreateSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
    {
        SPtr<SamplerState> ret = te_core_ptr<SamplerState>(nullptr); // TODO
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<BlendState> GLRenderStateManager::CreateBlendStateInternal(const BLEND_STATE_DESC& desc) const
    {
        SPtr<BlendState> ret = te_core_ptr<BlendState>(nullptr); // TODO
        ret->SetThisPtr(ret);

        return ret;
    }

	SPtr<RasterizerState> GLRenderStateManager::CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> ret = te_core_ptr<RasterizerState>(nullptr); // TODO
		ret->SetThisPtr(ret);

		return ret;
	}

	SPtr<DepthStencilState> GLRenderStateManager::CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> ret = te_core_ptr<DepthStencilState>(nullptr); // TODO
		ret->SetThisPtr(ret);

		return ret;
	}
}
