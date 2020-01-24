#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Describes a single GPU program data (for example int, float, Vector2) parameter. */
    struct GpuParamDataDesc
    {
        String Name;
        UINT32 ElementSize; /**< In multiples of 4 bytes. */
        UINT32 ArraySize;
        UINT32 ArrayElementStride; /**< In multiples of 4 bytes. */
        GpuParamDataType Type;

        UINT32 ParamBlockSlot;
        UINT32 ParamBlockSet;
        UINT32 GpuMemOffset; /**< In multiples of 4 bytes, or index for parameters not in a buffer. */
        UINT32 CpuMemOffset; /**< In multiples of 4 bytes. */
    };

    /** Describes a single GPU program object (for example texture, sampler state) parameter. */
    struct GpuParamObjectDesc
    {
        String Name;
        GpuParamObjectType Type;

        /** Slot within a set. Uniquely identifies bind location in the GPU pipeline, together with the set. */
        UINT32 Slot;

        /** Uniquely identifies the bind location in the GPU pipeline, together with the slot. */
        UINT32 Set;

        /** Underlying type of individual elements in the buffer or texture. */
        GpuBufferFormat ElementType = BF_UNKNOWN;
    };

    /** Describes a GPU program parameter block (collection of GPU program data parameters). */
    struct GpuParamBlockDesc
    {
        String Name;
        UINT32 Slot; /** Slot within a set. Uniquely identifies bind location in the GPU pipeline, together with the set. */
        UINT32 Set; /** Uniquely identifies the bind location in the GPU pipeline, together with the slot. */
        UINT32 BlockSize; /**< In multiples of 4 bytes. */
        bool IsShareable; /** True for blocks that can be shared between different GPU pipeline stages. */
    };

    /** Contains all parameter information for a GPU program, including data and object parameters, plus parameter blocks. */
    struct TE_CORE_EXPORT GpuParamDesc
    {
        Map<String, GpuParamBlockDesc> ParamBlocks;
        Map<String, GpuParamDataDesc> Params;

        Map<String, GpuParamObjectDesc> Samplers;
        Map<String, GpuParamObjectDesc> Textures;
        Map<String, GpuParamObjectDesc> Buffers;
    };
}
