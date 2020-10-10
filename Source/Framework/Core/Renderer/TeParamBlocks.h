#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeGpuParamBlockBuffer.h"

namespace te
{
    /** Wrapper for a single parameter in a parameter block buffer. */
    template<class T>
    class ParamBlockParam
    {
    public:
        ParamBlockParam() = default;
        ParamBlockParam(const GpuParamDataDesc& paramDesc)
            :_paramDesc(paramDesc)
        { }

        /**
         * Sets the parameter in the provided parameter block buffer. Caller is responsible for ensuring the param block
         * buffer contains this parameter.
         */
        void Set(const SPtr<GpuParamBlockBuffer>& paramBlock, const T& value, UINT32 arrayIdx = 0) const
        {
#if TE_DEBUG_MODE
            if (arrayIdx >= _paramDesc.ArraySize)
            {
                TE_ASSERT_ERROR(false, "Array index out of range. Array size: " +
                    ToString(_paramDesc.ArraySize) + ". Requested size: " + ToString(arrayIdx));
            }
#endif

            UINT32 elementSizeBytes = _paramDesc.ElementSize * sizeof(UINT32);
            UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T)); // Truncate if it doesn't fit within parameter size

            const bool transposeMatrices = gCaps().Convention.matrixOrder == Conventions::MatrixOrder::ColumnMajor;
            if (TransposePolicy<T>::TransposeEnabled(transposeMatrices))
            {
                auto transposed = TransposePolicy<T>::Transpose(value);
                paramBlock->Write((_paramDesc.CpuMemOffset + arrayIdx * _paramDesc.ArrayElementStride) * sizeof(UINT32),
                    &transposed, sizeBytes);
            }
            else
            {
                paramBlock->Write((_paramDesc.CpuMemOffset + arrayIdx * _paramDesc.ArrayElementStride) * sizeof(UINT32),
                    &value, sizeBytes);
            }

            // Set unused bytes to 0
            if (sizeBytes < elementSizeBytes)
            {
                UINT32 diffSize = elementSizeBytes - sizeBytes;
                paramBlock->ZeroOut((_paramDesc.CpuMemOffset + arrayIdx * _paramDesc.ArrayElementStride) * sizeof(UINT32) +
                    sizeBytes, diffSize);
            }
        }

        /**
         * Gets the parameter in the provided parameter block buffer. Caller is responsible for ensuring the param block
         * buffer contains this parameter.
         */
        T Get(GpuParamBlockBuffer* paramBlock, UINT32 arrayIdx = 0) const
        {
#if TE_DEBUG_MODE
            if (arrayIdx >= _paramDesc.ArraySize)
            {
                TE_DEBUG("Array index out of range. Array size: {" + ToString(_paramDesc.ArraySize) + "}. Requested size: {" + ToString(arrayIdx) + "}");
                return T();
            }
#endif

            UINT32 elementSizeBytes = _paramDesc.ElementSize * sizeof(UINT32);
            UINT32 sizeBytes = std::min(elementSizeBytes, (UINT32)sizeof(T));

            T value;
            paramBlock->Read((_paramDesc.CpuMemOffset + arrayIdx * _paramDesc.ArrayElementStride) * sizeof(UINT32), &value,
                sizeBytes);

            return value;
        }

        /**
         * Gets the parameter in the provided parameter block buffer. Caller is responsible for ensuring the param block
         * buffer contains this parameter.
         */
        T Get(const SPtr<GpuParamBlockBuffer>& paramBlock, UINT32 arrayIdx = 0) const
        {
            return Get(paramBlock.get(), arrayIdx);
        }

    protected:
        GpuParamDataDesc _paramDesc;
    };

    /** Base class for all parameter blocks. */
    struct TE_CORE_EXPORT ParamBlock
    {
        virtual ~ParamBlock();
        virtual void Initialize() = 0;
    };

    /**
     * Takes care of initializing param block definitions in a delayed manner since they depend on engine systems yet
     * are usually used as global variables which are initialized before engine systems are ready.
     */
    class TE_CORE_EXPORT ParamBlockManager : public Module<ParamBlockManager>
    {
    public:
        ParamBlockManager();

        /** Registers a new param block, and initializes it when ready. */
        static void RegisterBlock(ParamBlock* paramBlock);

        /** Removes the param block from the initialization list. */
        static void UnregisterBlock(ParamBlock* paramBlock);

    private:
        static Vector<ParamBlock*> sToInitialize;
    };

    /**
     * Starts a new custom parameter block. Custom parameter blocks allow you to create C++ structures that map directly
     * to GPU program buffers (for example uniform buffer in OpenGL or constant buffer in DX). Must be followed by
     * TE_PARAM_BLOCK_END.
     */
#define TE_PARAM_BLOCK_BEGIN(Name)																							\
	struct Name	: ParamBlock																								\
	{																														\
		Name()																												\
		{																													\
			ParamBlockManager::RegisterBlock(this);																			\
		}																													\
																															\
		SPtr<GpuParamBlockBuffer> CreateBuffer() const { return GpuParamBlockBuffer::Create(_blockSize); }					\
																															\
	private:																												\
		friend class ParamBlockManager;																						\
																															\
		void Initialize() override																							\
		{																													\
			_params = GetEntries();																							\
			RenderAPI& rapi = RenderAPI::Instance();																		\
																															\
			GpuParamBlockDesc blockDesc = rapi.GenerateParamBlockDesc(#Name, _params);										\
			_blockSize = blockDesc.BlockSize * sizeof(UINT32);																\
																															\
			InitEntries();																									\
		}																													\
																															\
		struct META_FirstEntry {};																							\
		static void META_GetPrevEntries(Vector<GpuParamDataDesc>& params, META_FirstEntry id) { }							\
		void META_InitPrevEntry(const Vector<GpuParamDataDesc>& params, UINT32 idx, META_FirstEntry id) { }					\
																															\
		typedef META_FirstEntry

     /**
      * Registers a new entry in a parameter block. Must be called in between TE_PARAM_BLOCK_BEGIN and TE_PARAM_BLOCK_END calls.
      */
#define TE_PARAM_BLOCK_ENTRY_ARRAY(TypeParam, NameParam, NumElements)														\
		META_Entry_##NameParam;																								\
																															\
		struct META_NextEntry_##NameParam {};																				\
		static void META_GetPrevEntries(Vector<GpuParamDataDesc>& params, META_NextEntry_##NameParam id)					\
		{																													\
			META_GetPrevEntries(params, META_Entry_##NameParam());															\
																															\
			params.push_back(GpuParamDataDesc());																			\
			GpuParamDataDesc& newEntry = params.back();																		\
			newEntry.Name = #NameParam;																						\
			newEntry.Type = (GpuParamDataType)TGpuDataParamInfo<TypeParam>::TypeId;											\
			newEntry.ArraySize = NumElements;																				\
			newEntry.ElementSize = sizeof(TypeParam);																		\
		}																													\
																															\
		void META_InitPrevEntry(const Vector<GpuParamDataDesc>& params, UINT32 idx, META_NextEntry_##NameParam id)			\
		{																													\
			META_InitPrevEntry(params, idx - 1, META_Entry_##NameParam());													\
			NameParam = ParamBlockParam<TypeParam>(params[idx]);															\
		}																													\
																															\
	public:																													\
		ParamBlockParam<TypeParam> NameParam;																				\
																															\
	private:																												\
		typedef META_NextEntry_##NameParam

      /**
       * Registers a new entry in a parameter block. Must be called in between TE_PARAM_BLOCK_BEGIN and TE_PARAM_BLOCK_END calls.
       */
#define TE_PARAM_BLOCK_ENTRY(Type, Name) TE_PARAM_BLOCK_ENTRY_ARRAY(Type, Name, 1)

       /** Ends parameter block definition. See TE_PARAM_BLOCK_BEGIN. */
#define TE_PARAM_BLOCK_END																									\
		META_LastEntry;																										\
																															\
		static Vector<GpuParamDataDesc> GetEntries()																		\
		{																													\
			Vector<GpuParamDataDesc> entries;																				\
			META_GetPrevEntries(entries, META_LastEntry());																	\
			return entries;																									\
		}																													\
																															\
		void InitEntries()																									\
		{																													\
			META_InitPrevEntry(_params, (UINT32)_params.size() - 1, META_LastEntry());										\
		}																													\
																															\
		Vector<GpuParamDataDesc> _params;																					\
		UINT32 _blockSize = 0;																								\
	};
}
