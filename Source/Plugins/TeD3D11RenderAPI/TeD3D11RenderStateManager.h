#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
	/**	Handles creation of DirectX 11 pipeline states. */
	class D3D11RenderStateManager : public RenderStateManager
	{
	protected:
        // TODO

		/** @copydoc RenderStateManager::createRasterizerStateInternal */
		SPtr<RasterizerState> CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const override;

		/** @copydoc RenderStateManager::createDepthStencilStateInternal */
		SPtr<DepthStencilState> CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const override;
	};
}
