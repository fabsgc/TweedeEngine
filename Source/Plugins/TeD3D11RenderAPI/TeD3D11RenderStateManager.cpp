#include "TeD3D11RenderStateManager.h"
#include "TeD3D11DepthStencilState.h"
#include "TeD3D11RasterizerState.h"

namespace te
{
	SPtr<RasterizerState> D3D11RenderStateManager::CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> ret = te_core_ptr<D3D11RasterizerState>(new (te_allocate<D3D11RasterizerState>()) D3D11RasterizerState(desc));
		ret->SetThisPtr(ret);

		return ret;
	}

	SPtr<DepthStencilState> D3D11RenderStateManager::CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> ret = te_core_ptr<D3D11DepthStencilState>(new (te_allocate<D3D11DepthStencilState>()) D3D11DepthStencilState(desc));
		ret->SetThisPtr(ret);

		return ret;
	}
}
