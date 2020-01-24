#include "TeGpuParams.h"
#include "TeHardwareBufferManager.h"
#include "TeGpuPipelineState.h"
#include "TeGpuParamDesc.h"

namespace te
{
    const TextureSurface GpuParams::COMPLETE = TextureSurface(0, 0, 0, 0);

    GpuParams::GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
        : _paramInfo(paramInfo)
    { }

    SPtr<GpuParams> GpuParams::Create(const SPtr<GraphicsPipelineState>& pipelineState, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(pipelineState->GetParamInfo(), deviceMask);
    }

    SPtr<GpuParams> GpuParams::Create(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(paramInfo, deviceMask);
    }

    SPtr<GpuParams> GpuParams::_getThisPtr() const
    {
        return std::static_pointer_cast<GpuParams>(GetThisPtr());
    }

    UINT32 GpuParams::GetDataParamSize(GpuProgramType type, const String& name) const
    {
        GpuParamDataDesc* desc = GetParamDesc(type, name);
        if (desc != nullptr)
        {
            return desc->ElementSize * 4;
        }

        return 0;
    }

    bool GpuParams::HasParam(GpuProgramType type, const String& name) const
    {
        return GetParamDesc(type, name) != nullptr;
    }

    bool GpuParams::HasTexture(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return false;
        }

        auto paramIter = paramDesc->Textures.find(name);
        if (paramIter != paramDesc->Textures.end())
        {
            return true;
        }

        return false;
    }

    bool GpuParams::HasBuffer(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return false;
        }

        auto paramIter = paramDesc->Buffers.find(name);
        if (paramIter != paramDesc->Buffers.end())
        {
            return true;
        }

        return false;
    }

    bool GpuParams::HasSamplerState(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return false;
        }

        auto paramIter = paramDesc->Samplers.find(name);
        if (paramIter != paramDesc->Samplers.end())
        {
            return true;
        }

        return false;
    }

    bool GpuParams::HasParamBlock(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return false;
        }

        auto paramBlockIter = paramDesc->ParamBlocks.find(name);
        if (paramBlockIter != paramDesc->ParamBlocks.end())
        {
            return true;
        }

        return false;
    }

    GpuParamBlockDesc* GpuParams::GetParamBlockDesc(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return nullptr;
        }

        auto paramBlockIter = paramDesc->ParamBlocks.find(name);
        if (paramBlockIter != paramDesc->ParamBlocks.end())
        {
            return &paramBlockIter->second;
        }

        return nullptr;
    }

    GpuParamDataDesc* GpuParams::GetParamDesc(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
        {
            return nullptr;
        }

        auto paramIter = paramDesc->Params.find(name);
        if (paramIter != paramDesc->Params.end())
        {
            return &paramIter->second;
        }

        return nullptr;
    }
}
