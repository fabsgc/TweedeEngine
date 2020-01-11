#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "TeDepthStencilState.h"
#include "TeRasterizerState.h"
#include "TeCommonTypes.h"

namespace te
{
	/** Descriptor structure used for initializing a GPU pipeline state. */
	struct PIPELINE_STATE_DESC
	{
        SPtr<RasterizerState> rasterizerState;
		SPtr<DepthStencilState> depthStencilState;

        // TODO
	};

	/**
	 * Describes the state of the GPU pipeline that determines how are primitives rendered. It consists of programmable
	 * states (vertex, fragment, geometry, etc. GPU programs), as well as a set of fixed states (blend, rasterizer,
	 * depth-stencil). Once created the state is immutable, and can be bound to RenderAPI for rendering.
	 */
	class TE_CORE_EXPORT GraphicsPipelineState : public CoreObject
	{
	public:
		virtual ~GraphicsPipelineState() = default;

        SPtr<RasterizerState> GetRasterizerState() const { return _data.rasterizerState; }
        SPtr<DepthStencilState> GetDepthStencilState() const { return _data.depthStencilState; }
        
        /** @copydoc RenderStateManager::CreateGraphicsPipelineState */
		static SPtr<GraphicsPipelineState> Create(const PIPELINE_STATE_DESC& desc);

	protected:
		friend class RenderStateManager;

		GraphicsPipelineState() = default;
		GraphicsPipelineState(const PIPELINE_STATE_DESC& desc);

        /** @copydoc CoreObject::Initialize() */
		void Initialize() override;

		PIPELINE_STATE_DESC _data;
	};
}
