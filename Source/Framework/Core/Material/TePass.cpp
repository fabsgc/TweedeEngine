#include "TePass.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeGpuPipelineState.h"

namespace te
{
    Pass::Pass()
    {
        _data.StencilRefValue = 0;
    }

    Pass::Pass(const PASS_DESC& desc)
        : _data(desc)
    { }

    Pass::~Pass()
    { }

    bool Pass::HasBlending() const
    {
        bool transparent = false;

        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            // Transparent if destination color is taken into account
            if (_data.BlendStateDesc.RenderTargetDesc[i].DstBlend != BF_ZERO ||
                _data.BlendStateDesc.RenderTargetDesc[i].SrcBlend == BF_DEST_COLOR ||
                _data.BlendStateDesc.RenderTargetDesc[i].SrcBlend == BF_INV_DEST_COLOR ||
                _data.BlendStateDesc.RenderTargetDesc[i].SrcBlend == BF_DEST_ALPHA ||
                _data.BlendStateDesc.RenderTargetDesc[i].SrcBlend == BF_INV_DEST_ALPHA)
            {
                transparent = true;
            }
        }

        return transparent;
    }

    const GPU_PROGRAM_DESC& Pass::GetProgramDesc(GpuProgramType type) const
    {
        switch (type)
        {
        default:
        case GPT_VERTEX_PROGRAM:
            return _data.VertexProgramDesc;
        case GPT_PIXEL_PROGRAM:
            return _data.PixelProgramDesc;
        case GPT_GEOMETRY_PROGRAM:
            return _data.GeometryProgramDesc;
        case GPT_HULL_PROGRAM:
            return _data.HullProgramDesc;
        case GPT_DOMAIN_PROGRAM:
            return _data.DomainProgramDesc;
        }
    }

    void Pass::CreatePipelineState()
    {
        GpuPipelineStateTypes::StateDescType desc;

        if (!_data.VertexProgramDesc.Source.empty())
            desc.vertexProgram = GpuProgram::Create(_data.VertexProgramDesc);

        if (!_data.PixelProgramDesc.Source.empty())
            desc.pixelProgram = GpuProgram::Create(_data.PixelProgramDesc);

        if (!_data.GeometryProgramDesc.Source.empty())
            desc.geometryProgram = GpuProgram::Create(_data.GeometryProgramDesc);

        if (!_data.HullProgramDesc.Source.empty())
            desc.hullProgram = GpuProgram::Create(_data.HullProgramDesc);

        if (!_data.DomainProgramDesc.Source.empty())
            desc.domainProgram = GpuProgram::Create(_data.DomainProgramDesc);

        desc.blendState = BlendState::Create(_data.BlendStateDesc);
        desc.rasterizerState = RasterizerState::Create(_data.RasterizerStateDesc);
        desc.depthStencilState = DepthStencilState::Create(_data.DepthStencilStateDesc);

        _graphicsPipelineState = GraphicsPipelineState::Create(desc);
    }

    void Pass::Compile()
    {
        if (_graphicsPipelineState)
            return; // Already compiled

        // Note: It's possible (and quite likely) the pass has already been compiled on the core thread, so this will
        // unnecessarily recompile it. However syncing them in a clean way is not trivial hard and this method is currently
        // not being used much (at all) to warrant a complex solution. Something to keep in mind for later though.
        CreatePipelineState();

        MarkCoreDirty();
    }

    SPtr<Pass> Pass::Create(const PASS_DESC& desc)
    {
        Pass* newPass = new (te_allocate<Pass>()) Pass(desc);
        SPtr<Pass> newPassPtr = te_core_ptr<Pass>(newPass);
        newPassPtr->SetThisPtr(newPassPtr);
        newPassPtr->Initialize();

        return newPassPtr;
    }

    SPtr<Pass> Pass::CreateEmpty()
    {
        Pass* newPass = new (te_allocate<Pass>()) Pass();
        SPtr<Pass> newPassPtr = te_core_ptr<Pass>(newPass);
        newPassPtr->SetThisPtr(newPassPtr);

        return newPassPtr;
    }

    void Pass::FrameSync()
    { }
}
