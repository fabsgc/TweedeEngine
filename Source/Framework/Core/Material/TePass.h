#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "Resources/TeResource.h"

namespace te
{
    /** Descriptor structure used for initializing a shader pass. */
	struct PASS_DESC
	{
		BLEND_STATE_DESC BlendStateDesc;
		RASTERIZER_STATE_DESC RasterizerStateDesc;
		DEPTH_STENCIL_STATE_DESC DepthStencilStateDesc;
		UINT32 StencilRefValue;

		GPU_PROGRAM_DESC VertexProgramDesc;
		GPU_PROGRAM_DESC PixelProgramDesc;
		GPU_PROGRAM_DESC GeometryProgramDesc;
		GPU_PROGRAM_DESC HullProgramDesc;
		GPU_PROGRAM_DESC DomainProgramDesc;
	};

    class TE_CORE_EXPORT Pass : public Resource
    {
    public:
        virtual ~Pass();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**	Returns true if this pass has some element of transparency. */
        bool HasBlending() const;

        /** Gets the stencil reference value that is used when performing operations using the stencil buffer. */
        UINT32 GetStencilRefValue() const { return _data.StencilRefValue; }

        /** Returns the GPU program descriptor for the specified GPU program type. */
        const GPU_PROGRAM_DESC& GetProgramDesc(GpuProgramType type) const;

        /**
         * Returns the graphics pipeline state describing this pass
         * Only valid after Compile() has been called.
         */
        const SPtr<GraphicsPipelineState>& GetGraphicsPipelineState() const { return _graphicsPipelineState; }

        /**
         * Returns the params created using the pipeline state
         * Only valid after Compile() has been called.
         */
        SPtr<GpuParams>& GetGpuParams() { return _gpuParams; }

        /** Creates either the graphics or the compute pipeline state from the stored pass data. */
        void CreatePipelineState();

        /**
         * Initializes the pass internals by compiling the GPU programs and creating the relevant pipeline state. This
         * method must be called before pass pipelines can be retrieved. After initial compilation further calls do this
         * method will perform no operation.
         */
        void Compile();

        /**	Creates a new empty pass. */
        static HPass Create(const PASS_DESC& desc);

        /** @copydoc Pass::Create */
        static SPtr<Pass> CreatePtr(const PASS_DESC& desc);

        /**	Creates a new empty pass but doesn't initialize it. */
        static SPtr<Pass> CreateEmpty();

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        Pass();
        Pass(const PASS_DESC & desc);

    protected:
        PASS_DESC _data;
		SPtr<GraphicsPipelineState> _graphicsPipelineState;
        SPtr<GpuParams> _gpuParams;
    };
}
