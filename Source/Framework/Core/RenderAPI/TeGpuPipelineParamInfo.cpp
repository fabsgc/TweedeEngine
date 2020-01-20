#include "RenderAPI/TeGpuPipelineParamInfo.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    GpuPipelineParamInfo::GpuPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc, GpuDeviceFlags deviceMask)
        : _numSets(0)
        , _numElements(0)
        , _setInfos(nullptr)
        , _resourceInfos()
    {
        te_zero_out(_numElementsPerType);

        _paramDescs[GPT_VERTEX_PROGRAM] = desc.VertexParams;
        _paramDescs[GPT_PIXEL_PROGRAM] = desc.PixelParams;
        _paramDescs[GPT_GEOMETRY_PROGRAM] = desc.GeometryParams;
        _paramDescs[GPT_HULL_PROGRAM] = desc.HullParams;
        _paramDescs[GPT_DOMAIN_PROGRAM] = desc.DomainParams;

        auto countElements = [&](auto& entry, ParamType type)
        {
            int typeIdx = (int)type;

            if ((entry.Set + 1) > _numSets)
            {
                _numSets = entry.Set + 1;
            }

            _numElementsPerType[typeIdx]++;
            _numElements++;
        };

        UINT32 numParamDescs = sizeof(_paramDescs) / sizeof(_paramDescs[0]);
        for (UINT32 i = 0; i < numParamDescs; i++)
        {
            const SPtr<GpuParamDesc>& paramDesc = _paramDescs[i];
            if (paramDesc == nullptr)
                continue;

            for (auto& paramBlock : paramDesc->ParamBlocks)
            {
                countElements(paramBlock.second, ParamType::ParamBlock);
            }

            for (auto& texture : paramDesc->Textures)
            {
                countElements(texture.second, ParamType::Texture);
            }

            for (auto& buffer : paramDesc->Buffers)
            {
                countElements(buffer.second, ParamType::Buffer);
            }

            for (auto& sampler : paramDesc->Samplers)
            {
                countElements(sampler.second, ParamType::SamplerState);
            }
        }

        UINT32* numSlotsPerSet = te_allocate<UINT32>(_numSets * sizeof(UINT32));
        te_zero_out(numSlotsPerSet, _numSets);

        for (UINT32 i = 0; i < numParamDescs; i++)
        {
            const SPtr<GpuParamDesc>& paramDesc = _paramDescs[i];
            if (paramDesc == nullptr)
                continue;

            for (auto& paramBlock : paramDesc->ParamBlocks)
            {
                numSlotsPerSet[paramBlock.second.Set] =
                    std::max(numSlotsPerSet[paramBlock.second.Set], paramBlock.second.Slot + 1);
            }

            for (auto& texture : paramDesc->Textures)
            {
                numSlotsPerSet[texture.second.Set] =
                    std::max(numSlotsPerSet[texture.second.Set], texture.second.Slot + 1);
            }

            for (auto& buffer : paramDesc->Buffers)
            {
                numSlotsPerSet[buffer.second.Set] =
                    std::max(numSlotsPerSet[buffer.second.Set], buffer.second.Slot + 1);
            }

            for (auto& sampler : paramDesc->Samplers)
            {
                numSlotsPerSet[sampler.second.Set] =
                    std::max(numSlotsPerSet[sampler.second.Set], sampler.second.Slot + 1);
            }
        }

        UINT32 totalNumSlots = 0;
        for (UINT32 i = 0; i < _numSets; i++)
        {
            totalNumSlots += numSlotsPerSet[i];
        }

        _setInfos = te_allocate<SetInfo>(_numSets * sizeof(SetInfo));

        if (_setInfos != nullptr)
        {
            te_zero_out(_setInfos, _numSets);
        }

        for (UINT32 i = 0; i < _numSets; i++)
        {
            _setInfos[i].NumSlots = numSlotsPerSet[i];
        }

        te_delete(numSlotsPerSet);

        for (UINT32 i = 0; i < _numSets; i++)
        {
            _setInfos[i].SlotIndices = te_allocate<UINT32>(_setInfos[i].NumSlots * sizeof(UINT32));
            memset(_setInfos[i].SlotIndices, -1, sizeof(UINT32) * _setInfos[i].NumSlots);

            _setInfos[i].SlotTypes = te_allocate<ParamType>(_setInfos[i].NumSlots * sizeof(ParamType));

            _setInfos[i].SlotSamplers = te_allocate<UINT32>(_setInfos[i].NumSlots * sizeof(UINT32));
            memset(_setInfos[i].SlotSamplers, -1, sizeof(UINT32) * _setInfos[i].NumSlots);
        }

        for (UINT32 i = 0; i < (UINT32)ParamType::Count; i++)
        {
            _resourceInfos[i] = te_allocate<ResourceInfo>(sizeof(ResourceInfo) * _numElementsPerType[i]);
            _numElementsPerType[i] = 0;
        }

        auto populateSetInfo = [&](auto& entry, ParamType type)
        {
            int typeIdx = (int)type;

            UINT32 sequentialIdx = _numElementsPerType[typeIdx];

            SetInfo& setInfo = _setInfos[entry.Set];
            setInfo.SlotIndices[entry.Slot] = sequentialIdx;
            setInfo.SlotTypes[entry.Slot] = type;

            _resourceInfos[typeIdx][sequentialIdx].Set = entry.Set;
            _resourceInfos[typeIdx][sequentialIdx].Slot = entry.Slot;

            _numElementsPerType[typeIdx]++;
        };

        for (UINT32 i = 0; i < numParamDescs; i++)
        {
            const SPtr<GpuParamDesc>& paramDesc = _paramDescs[i];
            if (paramDesc == nullptr)
            {
                continue;
            }

            for (auto& paramBlock : paramDesc->ParamBlocks)
            {
                populateSetInfo(paramBlock.second, ParamType::ParamBlock);
            }

            for (auto& texture : paramDesc->Textures)
            {
                populateSetInfo(texture.second, ParamType::Texture);
            }

            for (auto& buffer : paramDesc->Buffers)
            {
                populateSetInfo(buffer.second, ParamType::Buffer);
            }

            // Samplers need to be handled specially because certain slots could be texture/buffer + sampler combinations
            {
                int typeIdx = (int)ParamType::SamplerState;
                for (auto& entry : paramDesc->Samplers)
                {
                    const GpuParamObjectDesc& samplerDesc = entry.second;
                    UINT32 sequentialIdx = _numElementsPerType[typeIdx];

                    SetInfo& setInfo = _setInfos[samplerDesc.Set];
                    if (setInfo.SlotIndices[samplerDesc.Slot] == (UINT32)-1) // Slot is sampler only
                    {
                        setInfo.SlotIndices[samplerDesc.Slot] = sequentialIdx;
                        setInfo.SlotTypes[samplerDesc.Slot] = ParamType::SamplerState;
                    }
                    else // Slot is a combination
                    {
                        setInfo.SlotSamplers[samplerDesc.Slot] = sequentialIdx;
                    }

                    _resourceInfos[typeIdx][sequentialIdx].Set = samplerDesc.Set;
                    _resourceInfos[typeIdx][sequentialIdx].Slot = samplerDesc.Slot;

                    _numElementsPerType[typeIdx]++;
                }
            }
        }
    }

    UINT32 GpuPipelineParamInfo::GetSequentialSlot(ParamType type, UINT32 set, UINT32 slot) const
    {
#if TE_DEBUG_MODE
        if (set >= _numSets)
        {
            TE_DEBUG("Set index out of range: Valid range: [0, " + ToString(_numSets) + "). Requested: " + ToString(set), __FILE__, __LINE__);
            return -1;
        }

        if (slot >= _setInfos[set].NumSlots)
        {
            TE_DEBUG("Slot index out of range: Valid range: [0, {0}). Requested: {1}.", _setInfos[set].NumSlots, slot);
            return -1;
        }

        ParamType slotType = _setInfos[set].SlotTypes[slot];
        if (slotType != type)
        {
            // Allow sampler states & textures/buffers to share the same slot, as some APIs combine them
            if (type == ParamType::SamplerState)
            {
                if (_setInfos[set].SlotSamplers[slot] != (UINT32)-1)
                {
                    return _setInfos[set].SlotSamplers[slot];
                }
            }

            TE_DEBUG("Requested parameter is not of the valid type. Requested: " + ToString((UINT32)type) + 
                ". Actual: " + ToString((UINT32)_setInfos[set].SlotTypes[slot]), __FILE__, __LINE__);

            return -1;
        }

#endif

        return _setInfos[set].SlotIndices[slot];
    }

    void GpuPipelineParamInfo::GetBinding(ParamType type, UINT32 sequentialSlot, UINT32& set, UINT32& slot) const
    {
#if TE_DEBUG_MODE
        if (sequentialSlot >= _numElementsPerType[(int)type])
        {
            TE_DEBUG("Sequential slot index out of range: Valid range: [0, " + ToString(_numElementsPerType[(int)type]) + "). Requested: " +
                ToString(sequentialSlot), __FILE__, __LINE__);

            set = 0;
            slot = 0;
            return;
        }
#endif

        set = _resourceInfos[(int)type][sequentialSlot].Set;
        slot = _resourceInfos[(int)type][sequentialSlot].Slot;
    }

    void GpuPipelineParamInfo::GetBindings(ParamType type, const String& name, GpuParamBinding(&bindings)[GPT_COUNT])
    {
        constexpr UINT32 numParamDescs = sizeof(_paramDescs) / sizeof(_paramDescs[0]);
        static_assert(
            numParamDescs == GPT_COUNT,
            "Number of param descriptor structures must match the number of GPU program stages."
            );

        for (UINT32 i = 0; i < numParamDescs; i++)
        {
            GetBinding((GpuProgramType)i, type, name, bindings[i]);
        }
    }

    void GpuPipelineParamInfo::GetBinding(GpuProgramType progType, ParamType type, const String& name,
        GpuParamBinding& binding)
    {
        auto findBinding = [](auto& paramMap, const String& name, GpuParamBinding& binding)
        {
            auto iterFind = paramMap.find(name);
            if (iterFind != paramMap.end())
            {
                binding.set = iterFind->second.Set;
                binding.slot = iterFind->second.Slot;
            }
            else
                binding.set = binding.slot = (UINT32)-1;
        };

        const SPtr<GpuParamDesc>& paramDesc = _paramDescs[(UINT32)progType];
        if (paramDesc == nullptr)
        {
            binding.set = binding.slot = (UINT32)-1;
            return;
        }

        switch (type)
        {
        case ParamType::ParamBlock:
            findBinding(paramDesc->ParamBlocks, name, binding);
            break;
        case ParamType::Texture:
            findBinding(paramDesc->Textures, name, binding);
            break;
        case ParamType::Buffer:
            findBinding(paramDesc->Buffers, name, binding);
            break;
        case ParamType::SamplerState:
            findBinding(paramDesc->Samplers, name, binding);
            break;
        default:
            break;
        }
    }

    SPtr<GpuPipelineParamInfo> GpuPipelineParamInfo::Create(const GPU_PIPELINE_PARAMS_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        return RenderStateManager::Instance().CreatePipelineParamInfo(desc, deviceMask);
    }
}
