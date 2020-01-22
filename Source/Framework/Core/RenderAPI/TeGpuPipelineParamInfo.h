#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Helper structure used for initializing GpuPipelineParamInfo. */
    struct GPU_PIPELINE_PARAMS_DESC
    {
        SPtr<GpuParamDesc> PixelParams;
        SPtr<GpuParamDesc> VertexParams;
        SPtr<GpuParamDesc> GeometryParams;
        SPtr<GpuParamDesc> HullParams;
        SPtr<GpuParamDesc> DomainParams;
    };

    /** Binding location for a single GPU program parameter. */
    struct GpuParamBinding
    {
        UINT32 set = (UINT32)-1;
        UINT32 slot = (UINT32)-1;
    };

    /** Holds meta-data about a set of GPU parameters used by a single pipeline state.. */
    class TE_CORE_EXPORT GpuPipelineParamInfo : public CoreObject
    {
    public:
        /** Types of GPU parameters. */
        enum class ParamType
        {
            ParamBlock, Texture, Buffer, SamplerState, Count
        };

    public:
        virtual ~GpuPipelineParamInfo() = default;

        /** Gets the total number of sets. */
        UINT32 GetNumSets() const { return _numSets; }

        /** Returns the total number of elements across all sets. */
        UINT32 GetNumElements() const { return _numElements; }

        /** Returns the number of elements in all sets for the specified parameter type. */
        UINT32 GetNumElements(ParamType type) { return _numElementsPerType[(int)type]; }

        /**
         * Converts a set/slot combination into a sequential index that maps to the parameter in that parameter type's
         * array.
         *
         * If the set or slot is out of valid range, the method logs an error and returns -1. Only performs range checking
         * in debug mode.
         */
        UINT32 GetSequentialSlot(ParamType type, UINT32 set, UINT32 slot) const;

        /** Converts a sequential slot index into a set/slot combination. */
        void GetBinding(ParamType type, UINT32 sequentialSlot, UINT32& set, UINT32& slot) const;

        /**
         * Finds set/slot indices of a parameter with the specified name for the specified GPU program stage. Set/slot
         * indices are set to -1 if a stage doesn't have a block with the specified name.
         */
        void GetBinding(GpuProgramType progType, ParamType type, const String& name, GpuParamBinding& binding);

        /**
         * Finds set/slot indices of a parameter with the specified name for every GPU program stage. Set/slot indices are
         * set to -1 if a stage doesn't have a block with the specified name.
         */
        void GetBindings(ParamType type, const String& name, GpuParamBinding(&bindings)[GPT_COUNT]);

        /** Returns descriptions of individual parameters for the specified GPU program type. */
        const SPtr<GpuParamDesc>& GetParamDesc(GpuProgramType type) const { return _paramDescs[(int)type]; }

        /**
         * @copydoc te::GpuPipelineParamInfo::Create
         * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
         */
        static SPtr<GpuPipelineParamInfo> Create(const GPU_PIPELINE_PARAMS_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT);

    protected:
        /** Information about a single set in the param info object. */
        struct SetInfo
        {
            UINT32* SlotIndices;
            ParamType* SlotTypes;
            UINT32* SlotSamplers;
            UINT32 NumSlots;
        };

        /** Information how a resource maps to a certain set/slot. */
        struct ResourceInfo
        {
            UINT32 Set;
            UINT32 Slot;
        };

        std::array<SPtr<GpuParamDesc>, 5> _paramDescs;

        UINT32 _numSets;
        UINT32 _numElements;
        SetInfo* _setInfos;
        UINT32 _numElementsPerType[(int)ParamType::Count];
        ResourceInfo* _resourceInfos[(int)ParamType::Count];

    protected:
        friend class RenderStateManager;

        GpuPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc, GpuDeviceFlags deviceMask);

    };
}
