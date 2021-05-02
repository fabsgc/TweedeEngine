#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "TeGpuPipelineParamInfo.h"
#include "Utility/TeNonCopyable.h"
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
        SPtr<GpuProgram> pixelProgram;
        SPtr<GpuProgram> geometryProgram;
        SPtr<GpuProgram> hullProgram;
        SPtr<GpuProgram> domainProgram;
    };

    struct GpuPipelineStateTypes
    {
        typedef GpuPipelineParamInfo GpuPipelineParamInfoType;
        typedef PIPELINE_STATE_DESC StateDescType;
    };

    /**
     * Describes the state of the GPU pipeline that determines how are primitives rendered. It consists of programmable
     * states (vertex, pixel, geometry, etc. GPU programs), as well as a set of fixed states (blend, rasterizer,
     * depth-stencil). Once created the state is immutable, and can be bound to RenderAPI for rendering.
     */
    class TE_CORE_EXPORT GraphicsPipelineState : public CoreObject, public NonCopyable
    {
    public:
        using GpuPipelineParamInfoType = typename GpuPipelineStateTypes::GpuPipelineParamInfoType;

        virtual ~GraphicsPipelineState();

        SPtr<BlendState> GetBlendState() const { return _data.blendState; }
        SPtr<RasterizerState> GetRasterizerState() const { return _data.rasterizerState; }
        SPtr<DepthStencilState> GetDepthStencilState() const { return _data.depthStencilState; }

        bool HasVertexProgram() const { return _data.vertexProgram != nullptr; }
        bool HasPixelProgram() const { return _data.pixelProgram != nullptr; }
        bool HasGeometryProgram() const { return _data.geometryProgram != nullptr; }
        bool HasHullProgram() const { return _data.hullProgram != nullptr; }
        bool HasDomainProgram() const { return _data.domainProgram != nullptr; }

        const SPtr<GpuProgram>& GetVertexProgram() const { return _data.vertexProgram; }
        const SPtr<GpuProgram>& GetPixelProgram() const { return _data.pixelProgram; }
        const SPtr<GpuProgram>& GetGeometryProgram() const { return _data.geometryProgram; }
        const SPtr<GpuProgram>& GetHullProgram() const { return _data.hullProgram; }
        const SPtr<GpuProgram>& GetDomainProgram() const { return _data.domainProgram; }
        
        /** @copydoc RenderStateManager::CreateGraphicsPipelineState */
        static SPtr<GraphicsPipelineState> Create(const PIPELINE_STATE_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /** Returns an object containing meta-data for parameters of all GPU programs used in this pipeline state. */
        const SPtr<GpuPipelineParamInfoType>& GetParamInfo() const { return _paramInfo; }

    protected:
        friend class RenderStateManager;

        GraphicsPipelineState() = default;
        GraphicsPipelineState(const PIPELINE_STATE_DESC& desc, GpuDeviceFlags deviceMask);

        /** @copydoc CoreObject::Initialize() */
        void Initialize() override;

    protected:
        PIPELINE_STATE_DESC _data;
        GpuDeviceFlags _deviceMask = GDF_DEFAULT;
        SPtr<GpuPipelineParamInfoType> _paramInfo;
    };

    /**
     * Describes the state of the GPU pipeline that determines how are compute programs executed. It consists of
     * of a single programmable state (GPU program). Once created the state is immutable, and can be bound to RenderAPI for
     * use.
     */
    class TE_CORE_EXPORT ComputePipelineState : public CoreObject, public NonCopyable
    {
    public:
        using GpuPipelineParamInfoType = typename GpuPipelineStateTypes::GpuPipelineParamInfoType;

        virtual ~ComputePipelineState() = default;

        const SPtr<GpuProgram>& GetProgram() const { return _program; }

        /** Returns an object containing meta-data for parameters of the GPU program used in this pipeline state. */
        const SPtr<GpuPipelineParamInfoType>& GetParamInfo() const { return _paramInfo; }

        /** @copydoc RenderStateManager::CreateComputePipelineState */
        static SPtr<ComputePipelineState> Create(const SPtr<GpuProgram>& program, GpuDeviceFlags deviceMask = GDF_DEFAULT);

    protected:
        friend class RenderStateManager;

        ComputePipelineState();
        ComputePipelineState(const SPtr<GpuProgram>& program, GpuDeviceFlags deviceMask);

        /** @copydoc CoreObject::Initialize() */
        void Initialize() override;

    protected:
        SPtr<GpuProgram> _program;
        GpuDeviceFlags _deviceMask = GDF_DEFAULT;
        SPtr<GpuPipelineParamInfoType> _paramInfo;
    };
}
