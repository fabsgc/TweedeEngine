#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuPipelineState.h"

namespace te
{
	/** Handles creation of various render states. */
	class TE_CORE_EXPORT RenderStateManager : public Module <RenderStateManager>
	{
	public:
        // TODO

        RenderStateManager() = default;

		/** Creates and initializes a new DepthStencilState. */
		SPtr<DepthStencilState> CreateDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**	Creates and initializes a new RasterizerState. */
		SPtr<RasterizerState> CreateRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

		/** Creates and initializes a new GraphicsPipelineState. */
		SPtr<GraphicsPipelineState> CreateGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

        /** Creates an uninitialized depth-stencil state. Requires manual initialization after creation. */
		SPtr<DepthStencilState> _createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

        /** Creates an uninitialized rasterizer state. Requires manual initialization after creation. */
		SPtr<RasterizerState> _createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

        /**	Creates an uninitialized GraphicsPipelineState. Requires manual initialization after creation. */
		virtual SPtr<GraphicsPipelineState> _createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

        /**	Gets a rasterizer state initialized with default options. */
		const SPtr<RasterizerState>& GetDefaultRasterizerState() const;

		/**	Gets a depth stencil state initialized with default options. */
		const SPtr<DepthStencilState>& GetDefaultDepthStencilState() const;

    protected:
        /** @copydoc Module::OnShutDown */
		void OnShutDown() override;

        /** @copydoc createRasterizerState */
		virtual SPtr<RasterizerState> CreateRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const;

		/** @copydoc createDepthStencilState */
		virtual SPtr<DepthStencilState> CreateDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const;

    private:
		friend class RasterizerState;
		friend class DepthStencilState;
        friend class GraphicsPipelineState;

		mutable SPtr<RasterizerState> _defaultRasterizerState;
		mutable SPtr<DepthStencilState> _defaultDepthStencilState;
    };
}
