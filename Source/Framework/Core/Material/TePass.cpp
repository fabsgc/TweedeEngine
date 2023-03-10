#include "TePass.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuPipelineState.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResourceManager.h"
#include "Utility/TeDataStream.h"

namespace te
{
    Pass::Pass()
        : Serializable(TID_Pass)
    {
        _data.StencilRefValue = 0;
    }

    Pass::Pass(const PASS_DESC& desc)
        : Serializable(TID_Pass)
        , _data(desc)
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
        case GPT_COMPUTE_PROGRAM:
            return _data.ComputeProgramDesc;
        }
    }

    void Pass::CreatePipelineState(const ShaderVariation& variation)
    {
        if (IsCompute())
        {
            _data.ComputeProgramDesc.Variation = variation;
            SPtr<GpuProgram> program = GpuProgram::Create(_data.ComputeProgramDesc);
            _computePipelineState = ComputePipelineState::Create(program);
        }
        else
        {
            GpuPipelineStateTypes::StateDescType desc;

            if (!_data.VertexProgramDesc.Source.empty() && !desc.vertexProgram)
            {
                _data.VertexProgramDesc.Variation = variation;
                desc.vertexProgram = GpuProgram::Create(_data.VertexProgramDesc);
            }

            if (!_data.PixelProgramDesc.Source.empty() && !desc.pixelProgram)
            {
                _data.PixelProgramDesc.Variation = variation;
                desc.pixelProgram = GpuProgram::Create(_data.PixelProgramDesc);
            }

            if (!_data.GeometryProgramDesc.Source.empty() && !desc.geometryProgram)
            {
                _data.GeometryProgramDesc.Variation = variation;
                desc.geometryProgram = GpuProgram::Create(_data.GeometryProgramDesc);
            }

            if (!_data.HullProgramDesc.Source.empty() && !desc.hullProgram)
            {
                _data.HullProgramDesc.Variation = variation;
                desc.hullProgram = GpuProgram::Create(_data.HullProgramDesc);
            }

            if (!_data.DomainProgramDesc.Source.empty() && !desc.domainProgram)
            {
                _data.DomainProgramDesc.Variation = variation;
                desc.domainProgram = GpuProgram::Create(_data.DomainProgramDesc);
            }

            desc.blendState = BlendState::Create(_data.BlendStateDesc);
            desc.rasterizerState = RasterizerState::Create(_data.RasterizerStateDesc);
            desc.depthStencilState = DepthStencilState::Create(_data.DepthStencilStateDesc);

            _graphicsPipelineState = GraphicsPipelineState::Create(desc);
            _gpuParams = GpuParams::Create(_graphicsPipelineState);
        }
    }

    void Pass::Compile(const ShaderVariation& variation, bool force)
    {
        if ((_computePipelineState || _graphicsPipelineState) && !force)
            return; // Already compiled

        if (force) // if force is true, we update all GPU_PROGRAM_DESC source code
        {
            UpdateGpuProgramDesc(_data.VertexProgramDesc);
            UpdateGpuProgramDesc(_data.PixelProgramDesc);
            UpdateGpuProgramDesc(_data.GeometryProgramDesc);
            UpdateGpuProgramDesc(_data.HullProgramDesc);
            UpdateGpuProgramDesc(_data.DomainProgramDesc);
            UpdateGpuProgramDesc(_data.ComputeProgramDesc);
        }

        CreatePipelineState(variation);
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

    void Pass::UpdateGpuProgramDesc(GPU_PROGRAM_DESC& desc)
    {
        if (!desc.FilePath.empty())
        {
            FileStream shaderFile(desc.FilePath);
            desc.Source = shaderFile.GetAsString();
        }
    }
}
