#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "RenderAPI/TeBlendState.h"

namespace te
{
	/** Handles creation of various render states. */
	class TE_CORE_EXPORT RenderStateManager : public Module <RenderStateManager>
	{
	public:
        RenderStateManager() = default;

		/** Creates and initializes a new DepthStencilState. */
		SPtr<DepthStencilState> CreateDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**	Creates and initializes a new RasterizerState. */
		SPtr<RasterizerState> CreateRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

        /**	Creates and initializes a new BlendState. */
        SPtr<BlendState> CreateBlendState(const BLEND_STATE_DESC& desc) const;

		/** Creates and initializes a new GraphicsPipelineState. */
		SPtr<GraphicsPipelineState> CreateGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

        /** Creates an uninitialized depth-stencil state. Requires manual initialization after creation. */
		SPtr<DepthStencilState> _createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

        /** Creates an uninitialized rasterizer state. Requires manual initialization after creation. */
		SPtr<RasterizerState> _createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

        /**	Creates and initializes a new BlendState. */
        SPtr<BlendState> _createBlendState(const BLEND_STATE_DESC& desc) const;

        /**	Creates an uninitialized GraphicsPipelineState. Requires manual initialization after creation. */
		virtual SPtr<GraphicsPipelineState> _createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

        /**	Gets a rasterizer state initialized with default options. */
		const SPtr<RasterizerState>& GetDefaultRasterizerState() const;

		/**	Gets a depth stencil state initialized with default options. */
		const SPtr<DepthStencilState>& GetDefaultDepthStencilState() const;

        /**	Gets a blend state initialized with default options. */
        const SPtr<BlendState>& GetDefaultBlendState() const;

    protected:
        /** @copydoc Module::OnShutDown */
		void OnShutDown() override;

        /** @copydoc CreateRasterizerState */
		virtual SPtr<RasterizerState> CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const;

		/** @copydoc CreateDepthStencilState */
		virtual SPtr<DepthStencilState> CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const;

        /** @copydoc CreateDepthStencilState */
        virtual SPtr<BlendState> CreateBlendStateInternal(const BLEND_STATE_DESC& desc) const;

    private:
        friend class BlendState;
		friend class RasterizerState;
		friend class DepthStencilState;
        friend class GraphicsPipelineState;

        mutable SPtr<BlendState> _defaultBlendState;
		mutable SPtr<RasterizerState> _defaultRasterizerState;
		mutable SPtr<DepthStencilState> _defaultDepthStencilState;
    };
}
