#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuProgram.h"
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
        GPU_PROGRAM_DESC ComputeProgramDesc;
    };

    /**
     * Class defining a single pass of a technique (of a material). Pass may contain multiple GPU programs (vertex,
     * fragment, geometry, etc.), and a set of pipeline states (blend, rasterizer, etc.). When initially created the pass
     * is in its uncompiled state. It needs to be explicitly compiled by calling compile() before use.
     */
    class TE_CORE_EXPORT Pass : public CoreObject, public Serializable, public NonCopyable
    {
    public:
        virtual ~Pass() = default;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TypeID_Core::TID_Pass; }

        /**	Returns true if this pass has some element of transparency. */
        bool HasBlending() const;

        /** Returns true if the pass executes a compute program. */
        bool IsCompute() const { return !_data.ComputeProgramDesc.Source.empty(); }

        /** Gets the stencil reference value that is used when performing operations using the stencil buffer. */
        UINT32 GetStencilRefValue() const { return _data.StencilRefValue; }

        /** Returns the GPU program descriptor for the specified GPU program type. */
        const GPU_PROGRAM_DESC& GetProgramDesc(GpuProgramType type) const;

        /**
         * Returns the graphics pipeline state describing this pass, or null if its a compute pass.
         * Only valid after Compile() has been called.
         */
        const SPtr<GraphicsPipelineState>& GetGraphicsPipelineState() const { return _graphicsPipelineState; }

        /**
         * Returns the compute pipeline state describing this pass, or null if its a graphics pass.
         * Only valid after compile has been called.
         */
        const SPtr<ComputePipelineState>& GetComputePipelineState() const { return _computePipelineState; }

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
         * 
         * @param[in] force Force compile even if pass has already been built
         */
        void Compile(bool force = false);

        /**	Creates a new empty pass. */
        static SPtr<Pass> Create(const PASS_DESC& desc);

        /**	Creates a new empty pass but doesn't initialize it. */
        static SPtr<Pass> CreateEmpty();

    protected:
        Pass();
        Pass(const PASS_DESC & desc);

        void UpdateGpuProgramDesc(GPU_PROGRAM_DESC& desc);

    protected:
        PASS_DESC _data;
        SPtr<GraphicsPipelineState> _graphicsPipelineState;
        SPtr<ComputePipelineState> _computePipelineState;
        SPtr<GpuParams> _gpuParams;
    };
}
