#include "TePass.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"

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

    void Pass::Initialize()
    {
        CoreObject::Initialize();
    }

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

        if (!_data.VertexProgramDesc.Source.empty() && !desc.vertexProgram)
            desc.vertexProgram = GpuProgram::Create(_data.VertexProgramDesc);

        if (!_data.PixelProgramDesc.Source.empty() && !desc.pixelProgram)
            desc.pixelProgram = GpuProgram::Create(_data.PixelProgramDesc);

        if (!_data.GeometryProgramDesc.Source.empty() && !desc.geometryProgram)
            desc.geometryProgram = GpuProgram::Create(_data.GeometryProgramDesc);

        if (!_data.HullProgramDesc.Source.empty() && !desc.hullProgram)
            desc.hullProgram = GpuProgram::Create(_data.HullProgramDesc);

        if (!_data.DomainProgramDesc.Source.empty() && !desc.domainProgram)
            desc.domainProgram = GpuProgram::Create(_data.DomainProgramDesc);

        desc.blendState = BlendState::Create(_data.BlendStateDesc);
        desc.rasterizerState = RasterizerState::Create(_data.RasterizerStateDesc);
        desc.depthStencilState = DepthStencilState::Create(_data.DepthStencilStateDesc);

        _graphicsPipelineState = GraphicsPipelineState::Create(desc);
        _gpuParams = GpuParams::Create(_graphicsPipelineState);
    }

    void Pass::Compile()
    {
        if (_graphicsPipelineState)
            return; // Already compiled

        CreatePipelineState();
        MarkCoreDirty();
    }

    HPass Pass::Create(const PASS_DESC& desc)
    {
        const SPtr<Pass> passPtr = CreatePtr(desc);
        passPtr->Initialize();

        return static_resource_cast<Pass>(gResourceManager()._createResourceHandle(passPtr));
    }

    SPtr<Pass> Pass::CreatePtr(const PASS_DESC& desc)
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
