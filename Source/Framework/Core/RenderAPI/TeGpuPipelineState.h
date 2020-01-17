#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "TeDepthStencilState.h"
#include "TeRasterizerState.h"
#include "TeBlendState.h"
#include "TeCommonTypes.h"

namespace te
{
	/** Descriptor structure used for initializing a GPU pipeline state. */
	struct PIPELINE_STATE_DESC
	{
        SPtr<BlendState> blendState;
        SPtr<RasterizerState> rasterizerState;
		SPtr<DepthStencilState> depthStencilState;

        SPtr<GpuProgram> vertexProgram;
        SPtr<GpuProgram> fragmentProgram;
        SPtr<GpuProgram> geometryProgram;
        SPtr<GpuProgram> hullProgram;
        SPtr<GpuProgram> domainProgram;
	};

	/**
	 * Describes the state of the GPU pipeline that determines how are primitives rendered. It consists of programmable
	 * states (vertex, pixel, geometry, etc. GPU programs), as well as a set of fixed states (blend, rasterizer,
	 * depth-stencil). Once created the state is immutable, and can be bound to RenderAPI for rendering.
	 */
	class TE_CORE_EXPORT GraphicsPipelineState : public CoreObject
	{
	public:
		virtual ~GraphicsPipelineState() = default;

        SPtr<BlendState> GetBlendState() const { return _data.blendState; }
        SPtr<RasterizerState> GetRasterizerState() const { return _data.rasterizerState; }
        SPtr<DepthStencilState> GetDepthStencilState() const { return _data.depthStencilState; }

        bool HasVertexProgram() const { return _data.vertexProgram != nullptr; }
        bool HasFragmentProgram() const { return _data.fragmentProgram != nullptr; }
        bool HasGeometryProgram() const { return _data.geometryProgram != nullptr; }
        bool HasHullProgram() const { return _data.hullProgram != nullptr; }
        bool HasDomainProgram() const { return _data.domainProgram != nullptr; }

        const SPtr<GpuProgram>& GetVertexProgram() const { return _data.vertexProgram; }
        const SPtr<GpuProgram>& GetFragmentProgram() const { return _data.fragmentProgram; }
        const SPtr<GpuProgram>& GetGeometryProgram() const { return _data.geometryProgram; }
        const SPtr<GpuProgram>& GetHullProgram() const { return _data.hullProgram; }
        const SPtr<GpuProgram>& GetDomainProgram() const { return _data.domainProgram; }
        
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
