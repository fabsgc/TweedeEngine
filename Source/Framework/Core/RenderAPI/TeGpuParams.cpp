#include "TeGpuParams.h"
#include "TeHardwareBufferManager.h"
#include "TeGpuParamBlockBuffer.h"
#include "TeGpuPipelineState.h"
#include "TeGpuParamDesc.h"
#include "TeSamplerState.h"
#include "TeGpuBuffer.h"
#include "Image/TeTexture.h"

namespace te
{
    const TextureSurface GpuParams::COMPLETE = TextureSurface(0, 0, 0, 0);

    GpuParams::GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
        : _paramInfo(paramInfo)
    {
        UINT32 numParamBlocks = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::ParamBlock);
        UINT32 numTextures = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Texture);
        UINT32 numBuffers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Buffer);
        UINT32 numSamplers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::SamplerState);

        UINT32 paramBlocksSize = sizeof(GpuParamBlockBuffer) * numParamBlocks;
        UINT32 texturesSize = (sizeof(Texture) + sizeof(TextureSurface)) * numTextures;
        UINT32 buffersSize = sizeof(GpuBuffer) * numBuffers;
        UINT32 samplerStatesSize = sizeof(SamplerState) * numSamplers;

        UINT32 totalSize = paramBlocksSize + texturesSize + buffersSize + samplerStatesSize;

        UINT8* data = (UINT8*)te_allocate(totalSize);
        _paramBlockBuffers = (SPtr<GpuParamBlockBuffer>*)data;
        for (UINT32 i = 0; i < numParamBlocks; i++)
        {
            new (&_paramBlockBuffers[i]) SPtr<GpuParamBlockBuffer>();
        }
        data += paramBlocksSize;

        _sampledTextureData = (TextureData*)data;
        for (UINT32 i = 0; i < numTextures; i++)
        {
            new (&_sampledTextureData[i].Tex) SPtr<Texture>();
            new (&_sampledTextureData[i].Surface) TextureSurface(0, 0, 0, 0);
        }
        data += texturesSize;

        _buffers = (SPtr<GpuBuffer>*)data;
        for (UINT32 i = 0; i < numBuffers; i++)
        {
            new (&_buffers[i]) SPtr<GpuBuffer>();
        }
        data += buffersSize;

        _samplerStates = (SPtr<SamplerState>*)data;
        for (UINT32 i = 0; i < numSamplers; i++)
        {
            new (&_samplerStates[i]) SPtr<SamplerState>();
        }
        data += samplerStatesSize;
    }

    GpuParams::~GpuParams()
    {
        UINT32 numParamBlocks = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::ParamBlock);
        UINT32 numTextures = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Texture);
        UINT32 numBuffers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Buffer);
        UINT32 numSamplers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::SamplerState);

        for (UINT32 i = 0; i < numParamBlocks; i++)
        {
            _paramBlockBuffers[i].~shared_ptr();
        }

        for (UINT32 i = 0; i < numTextures; i++)
        {
            _sampledTextureData[i].Tex.~shared_ptr();
            _sampledTextureData[i].Surface.~TextureSurface();
        }

        for (UINT32 i = 0; i < numBuffers; i++)
        {
            _buffers[i].~shared_ptr();
        }

        for (UINT32 i = 0; i < numSamplers; i++)
        {
            _samplerStates[i].~shared_ptr();
        }

        // Everything is allocated in a single block, so it's enough to free the first element
        te_free(_paramBlockBuffers);
    }

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

    SPtr<GpuParamBlockBuffer> GpuParams::GetParamBlockBuffer(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::ParamBlock, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("GpuParamBlockBuffer not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return nullptr;
        }

        return _paramBlockBuffers[globalSlot];
    }

    SPtr<Texture> GpuParams::GetTexture(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Texture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("Texture not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return nullptr;
        }

        return _sampledTextureData[globalSlot].Tex;
    }

    SPtr<GpuBuffer> GpuParams::GetBuffer(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Buffer, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("GpuBuffer not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return nullptr;
        }

        return _buffers[globalSlot];
    }

    SPtr<SamplerState> GpuParams::GetSamplerState(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::SamplerState, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("SamplerState not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return nullptr;
        }

        return _samplerStates[globalSlot];
    }

    const TextureSurface& GpuParams::GetTextureSurface(UINT32 set, UINT32 slot) const
    {
        static TextureSurface emptySurface;

        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Texture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("Texture surface not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return emptySurface;
        }

        return _sampledTextureData[globalSlot].Surface;
    }

    void GpuParams::SetParamBlockBuffer(UINT32 set, UINT32 slot, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::ParamBlock, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("ParamBlockBuffer can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return;
        }

        _paramBlockBuffers[globalSlot] = paramBlockBuffer;
        _hasChanged = true;
    }

    void GpuParams::SetParamBlockBuffer(GpuProgramType type, const String& name, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer)
    {
        const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc(type);
        if (paramDescs == nullptr)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}", __FILE__, __LINE__);
            return;
        }

        auto iterFind = paramDescs->ParamBlocks.find(name);
        if (iterFind == paramDescs->ParamBlocks.end())
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}", __FILE__, __LINE__);
            return;
        }

        SetParamBlockBuffer(iterFind->second.Set, iterFind->second.Slot, paramBlockBuffer);
    }

    void GpuParams::SetParamBlockBuffer(const String& name, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer)
    {
        bool found = false;
        for (UINT32 i = 0; i < 6; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->ParamBlocks.find(name);
            if (iterFind == paramDescs->ParamBlocks.end())
            {
                continue;
            }
            
            found = true;
            SetParamBlockBuffer(iterFind->second.Set, iterFind->second.Slot, paramBlockBuffer);
        }

        if (!found)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}", __FILE__, __LINE__);
        }
    }

    void GpuParams::SetTexture(UINT32 set, UINT32 slot, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Texture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            return;
        }

        _sampledTextureData[globalSlot].Tex = texture;
        _sampledTextureData[globalSlot].Surface = surface;
        _hasChanged = true;
    }

    void GpuParams::SetBuffer(UINT32 set, UINT32 slot, const SPtr<GpuBuffer>& buffer)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Buffer, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("GpuBuffer can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return;
        }

        _buffers[globalSlot] = buffer;
        _hasChanged = true;
    }

    void GpuParams::SetSamplerState(UINT32 set, UINT32 slot, const SPtr<SamplerState>& sampler)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::SamplerState, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("SamplerState can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}", __FILE__, __LINE__);
            return;
        }

        _samplerStates[globalSlot] = sampler;
        _hasChanged = true;
    }
}
