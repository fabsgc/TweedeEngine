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
    const GpuDataParamInfos GpuParams::PARAM_SIZES;

    GpuParams::GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
        : _paramInfo(paramInfo)
    {
        UINT32 numParamBlocks = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::ParamBlock);
        UINT32 numTextures = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Texture);
        UINT32 numStorageTextures = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::LoadStoreTexture);
        UINT32 numBuffers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Buffer);
        UINT32 numSamplers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::SamplerState);

        UINT32 paramBlocksSize = sizeof(GpuParamBlockBuffer) * numParamBlocks;
        UINT32 texturesSize = (sizeof(Texture) + sizeof(TextureSurface)) * numTextures;
        UINT32 loadStoreTexturesSize = (sizeof(TextureType) + sizeof(TextureSurface)) * numStorageTextures;
        UINT32 buffersSize = sizeof(GpuBuffer) * numBuffers;
        UINT32 samplerStatesSize = sizeof(SamplerState) * numSamplers;

        UINT32 totalSize = paramBlocksSize + texturesSize + loadStoreTexturesSize + buffersSize + samplerStatesSize;

        UINT8* data = (UINT8*)te_allocate(totalSize);
        _paramBlockBuffers = (SPtr<GpuParamBlockBuffer>*)data;
        for (UINT32 i = 0; i < numParamBlocks; i++)
        {
            new (&_paramBlockBuffers[i]) SPtr<GpuParamBlockBuffer>();

            if (!_paramBlockBuffers)
            {
                TE_PRINT("FAILED TO CREATE PARAM BLOCK BUFFER");
            }
        }
        data += paramBlocksSize;

        _sampledTextureData = (TextureData*)data;
        for (UINT32 i = 0; i < numTextures; i++)
        {
            new (&_sampledTextureData[i].Tex) SPtr<Texture>();
            new (&_sampledTextureData[i].Surface) TextureSurface(0, 0, 0, 0);
        }
        data += texturesSize;

        _loadStoreTextureData = (TextureData*)data;
        for (UINT32 i = 0; i < numStorageTextures; i++)
        {
            new (&_loadStoreTextureData[i].Tex) TextureType();
            new (&_loadStoreTextureData[i].Surface) TextureSurface(0, 0, 0, 0);
        }
        data += loadStoreTexturesSize;

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
        UINT32 numStorageTextures = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::LoadStoreTexture);
        UINT32 numBuffers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::Buffer);
        UINT32 numSamplers = _paramInfo->GetNumElements(GpuPipelineParamInfo::ParamType::SamplerState);

        for (UINT32 i = 0; i < numParamBlocks; i++)
            _paramBlockBuffers[i].~shared_ptr();

        for (UINT32 i = 0; i < numTextures; i++)
        {
            _sampledTextureData[i].Tex.~shared_ptr();
            _sampledTextureData[i].Surface.~TextureSurface();
        }

        for (UINT32 i = 0; i < numStorageTextures; i++)
        {
            _loadStoreTextureData[i].Tex.~shared_ptr();
            _loadStoreTextureData[i].Surface.~TextureSurface();
        }

        for (UINT32 i = 0; i < numBuffers; i++)
            _buffers[i].~shared_ptr();

        for (UINT32 i = 0; i < numSamplers; i++)
            _samplerStates[i].~shared_ptr();

        // Everything is allocated in a single block, so it's enough to free the first element
        te_free(_paramBlockBuffers);
    }

    SPtr<GpuParams> GpuParams::Create(const SPtr<GraphicsPipelineState>& pipelineState, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(pipelineState->GetParamInfo(), deviceMask);
    }

    SPtr<GpuParams> GpuParams::Create(const SPtr<ComputePipelineState>& pipelineState, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(pipelineState->GetParamInfo());
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
            return desc->ElementSize * 4;

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

    bool GpuParams::HasLoadStoreTexture(GpuProgramType type, const String& name) const
    {
        const SPtr<GpuParamDesc>& paramDesc = _paramInfo->GetParamDesc(type);
        if (paramDesc == nullptr)
            return false;

        auto paramIter = paramDesc->LoadStoreTextures.find(name);
        if (paramIter != paramDesc->LoadStoreTextures.end())
            return true;

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
            TE_DEBUG("GpuParamBlockBuffer not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return nullptr;
        }

        return _paramBlockBuffers[globalSlot];
    }

    SPtr<Texture> GpuParams::GetTexture(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Texture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("Texture not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return nullptr;
        }

        return _sampledTextureData[globalSlot].Tex;
    }

    SPtr<Texture> GpuParams::GetLoadStoreTexture(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::LoadStoreTexture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("Load Store Texture not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return nullptr;
        }

        return _loadStoreTextureData[globalSlot].Tex;
    }

    SPtr<GpuBuffer> GpuParams::GetBuffer(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Buffer, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("GpuBuffer not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return nullptr;
        }

        return _buffers[globalSlot];
    }

    SPtr<SamplerState> GpuParams::GetSamplerState(UINT32 set, UINT32 slot) const
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::SamplerState, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("SamplerState not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
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
            TE_DEBUG("Texture surface not found in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return emptySurface;
        }

        return _sampledTextureData[globalSlot].Surface;
    }

    const TextureSurface& GpuParams::GetLoadStoreSurface(UINT32 set, UINT32 slot) const
    {
        static TextureSurface emptySurface;

        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::LoadStoreTexture, set, slot);
        if (globalSlot == (UINT32)-1)
            return emptySurface;

        return _loadStoreTextureData[globalSlot].Surface;
    }

    void GpuParams::SetParam(GpuProgramType type, const String& name, const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
    {
        if (!HasParam(type, name))
        {
            TE_PRINT("GpuProgram {" + ToString((UINT32)type) + "} does not have {" + name + "} parameter");
            return;
        }

        GpuParamDataDesc* desc = GetParamDesc(type, name);

        SPtr<GpuParamBlockBuffer> paramBlock = GetParamBlockBuffer(desc->ParamBlockSet, desc->ParamBlockSlot);
        if (paramBlock == nullptr)
            return;

        UINT32 elementSizeBytes = desc->ElementSize * sizeof(UINT32);

#if TE_DEBUG_MODE
        if (sizeBytes > elementSizeBytes)
        {
            TE_DEBUG("Provided element size larger than maximum element size. Maximum size: {" + ToString(elementSizeBytes) + "}."
                " Supplied size: {" + ToString(sizeBytes) + "}");
        }

        if (arrayIdx >= desc->ArraySize)
        {
            TE_ASSERT_ERROR(false, "Array index out of range. Array size: " +
                ToString(desc->ArraySize) + ". Requested size: " + ToString(arrayIdx));
        }
#endif

        sizeBytes = std::min(elementSizeBytes, sizeBytes);

        paramBlock->Write((desc->CpuMemOffset + arrayIdx * desc->ArrayElementStride) * sizeof(UINT32), value, sizeBytes);

        // Set unused bytes to 0
        if (sizeBytes < elementSizeBytes)
        {
            UINT32 diffSize = elementSizeBytes - sizeBytes;
            paramBlock->ZeroOut((desc->CpuMemOffset + arrayIdx * desc->ArrayElementStride) * sizeof(UINT32) + sizeBytes, diffSize);
        }
    }

    void GpuParams::SetParam(const String& name, const void* value, UINT32 sizeBytes, UINT32 arrayIdx)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->Params.find(name);
            if (iterFind == paramDescs->Params.end())
            {
                continue;
            }

            SetParam((GpuProgramType)i, name, value, sizeBytes);
        }
    }

    void GpuParams::SetParamBlockBuffer(UINT32 set, UINT32 slot, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::ParamBlock, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("ParamBlockBuffer can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
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
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        auto iterFind = paramDescs->ParamBlocks.find(name);
        if (iterFind == paramDescs->ParamBlocks.end())
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        SetParamBlockBuffer(iterFind->second.Set, iterFind->second.Slot, paramBlockBuffer);
    }

    void GpuParams::SetParamBlockBuffer(const String& name, const SPtr<GpuParamBlockBuffer>& paramBlockBuffer)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
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
            
            SetParamBlockBuffer(iterFind->second.Set, iterFind->second.Slot, paramBlockBuffer);
        }
    }

    void GpuParams::SetTexture(GpuProgramType type, const String& name, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc(type);
        if (paramDescs == nullptr)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        auto iterFind = paramDescs->Textures.find(name);
        if (iterFind == paramDescs->Textures.end())
        {
            TE_DEBUG("Cannot find texture with the name: {" + name + "}");
            return;
        }

        SetTexture(iterFind->second.Set, iterFind->second.Slot, texture, surface);
    }

    void GpuParams::SetTexture(const String& name, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->Textures.find(name);
            if (iterFind == paramDescs->Textures.end())
            {
                continue;
            }

            SetTexture(iterFind->second.Set, iterFind->second.Slot, texture, surface);
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

    void GpuParams::SetLoadStoreTexture(GpuProgramType type, const String& name, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc(type);
        if (paramDescs == nullptr)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        auto iterFind = paramDescs->LoadStoreTextures.find(name);
        if (iterFind == paramDescs->LoadStoreTextures.end())
        {
            TE_DEBUG("Cannot find texture with the name: {" + name + "}");
            return;
        }

        SetTexture(iterFind->second.Set, iterFind->second.Slot, texture, surface);
    }

    void GpuParams::SetLoadStoreTexture(const String& name, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->LoadStoreTextures.find(name);
            if (iterFind == paramDescs->LoadStoreTextures.end())
            {
                continue;
            }

            SetLoadStoreTexture(iterFind->second.Set, iterFind->second.Slot, texture, surface);
        }
    }

    void GpuParams::SetLoadStoreTexture(UINT32 set, UINT32 slot, const SPtr<Texture>& texture, const TextureSurface& surface)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::LoadStoreTexture, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            return;
        }

        _loadStoreTextureData[globalSlot].Tex = texture;
        _loadStoreTextureData[globalSlot].Surface = surface;
        _hasChanged = true;
    }

    void GpuParams::SetBuffer(GpuProgramType type, const String& name, const SPtr<GpuBuffer>& buffer)
    {
        const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc(type);
        if (paramDescs == nullptr)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        auto iterFind = paramDescs->Buffers.find(name);
        if (iterFind == paramDescs->Buffers.end())
        {
            TE_DEBUG("Cannot find gpu buffer with the name: {" + name + "}");
            return;
        }

        SetBuffer(iterFind->second.Set, iterFind->second.Slot, buffer);
    }

    void GpuParams::SetBuffer(const String& name, const SPtr<GpuBuffer>& buffer)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->Buffers.find(name);
            if (iterFind == paramDescs->Buffers.end())
            {
                continue;
            }

            SetBuffer(iterFind->second.Set, iterFind->second.Slot, buffer);
        }
    }

    void GpuParams::SetBuffer(UINT32 set, UINT32 slot, const SPtr<GpuBuffer>& buffer)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::Buffer, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("GpuBuffer can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return;
        }

        _buffers[globalSlot] = buffer;
        _hasChanged = true;
    }

    void GpuParams::SetSamplerState(GpuProgramType type, const String& name, const SPtr<SamplerState>& sampler)
    {
        const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc(type);
        if (paramDescs == nullptr)
        {
            TE_DEBUG("Cannot find parameter block with the name: {" + name + "}");
            return;
        }

        auto iterFind = paramDescs->Samplers.find(name);
        if (iterFind == paramDescs->Samplers.end())
        {
            TE_DEBUG("Cannot find sampler state with the name: {" + name + "}");
            return;
        }

        SetSamplerState(iterFind->second.Set, iterFind->second.Slot, sampler);
    }

    void GpuParams::SetSamplerState(const String& name, const SPtr<SamplerState>& sampler)
    {
        for (UINT32 i = 0; i < GPT_COUNT; i++)
        {
            const SPtr<GpuParamDesc>& paramDescs = _paramInfo->GetParamDesc((GpuProgramType)i);
            if (paramDescs == nullptr)
            {
                continue;
            }

            auto iterFind = paramDescs->Samplers.find(name);
            if (iterFind == paramDescs->Samplers.end())
            {
                continue;
            }

            SetSamplerState(iterFind->second.Set, iterFind->second.Slot, sampler);
        }
    }

    void GpuParams::SetSamplerState(UINT32 set, UINT32 slot, const SPtr<SamplerState>& sampler)
    {
        UINT32 globalSlot = _paramInfo->GetSequentialSlot(GpuPipelineParamInfo::ParamType::SamplerState, set, slot);
        if (globalSlot == (UINT32)-1)
        {
            TE_DEBUG("SamplerState can't be set in set {" + ToString(set) + "} and slot {" + ToString(slot) + "}");
            return;
        }

        _samplerStates[globalSlot] = sampler;
        _hasChanged = true;
    }
}
