#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    /** Handles creation of OpenGL pipeline states. */
    class GLRenderStateManager : public RenderStateManager
    {
    protected:
        /** @copydoc RenderStateManager::CreateSamplerStateInternal */
        SPtr<SamplerState> CreateSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const override;

        /** @copydoc RenderStateManager::CreateBlendStateInternal */
        SPtr<BlendState> CreateBlendStateInternal(const BLEND_STATE_DESC& desc) const override;

        /** @copydoc RenderStateManager::CreateRasterizerStateInternal */
        SPtr<RasterizerState> CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const override;

        /** @copydoc RenderStateManager::CreateDepthStencilStateInternal */
        SPtr<DepthStencilState> CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const override;
    };
}
