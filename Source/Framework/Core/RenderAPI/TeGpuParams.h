#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParam.h"
#include "CoreUtility/TeCoreObject.h"
#include "Math/TeMatrixNxM.h"
#include "TeGpuPipelineParamInfo.h"
#include "TeGpuBuffer.h"

namespace te
{
	class TE_CORE_EXPORT GpuParams : public CoreObject
	{
	public:
		/** Surface that covers all texture sub-resources. */
		static const TextureSurface COMPLETE;

		virtual ~GpuParams() = default;

		/** Returns a description of all stored parameters. */
		SPtr<GpuParamDesc> GetParamDesc(GpuProgramType type) const { return _paramInfo->GetParamDesc(type); }

		/** Gets the object that contains the processed information about all parameters. */
		SPtr<GpuPipelineParamInfo> GetParamInfo() const { return _paramInfo; }

		/** Returns the size of a data parameter with the specified name, in bytes. Returns 0 if such parameter doesn't exist. */
		UINT32 GetDataParamSize(GpuProgramType type, const String& name) const;

		/** Checks if parameter with the specified name exists. */
		bool HasParam(GpuProgramType type, const String& name) const;

		/**	Checks if texture parameter with the specified name exists. */
		bool HasTexture(GpuProgramType type, const String& name) const;

		/**	Checks if buffer parameter with the specified name exists. */
		bool HasBuffer(GpuProgramType type, const String& name) const;

		/**	Checks if sampler state parameter with the specified name exists. */
		bool HasSamplerState(GpuProgramType type, const String& name) const;

		/** Checks if a parameter block with the specified name exists. */
		bool HasParamBlock(GpuProgramType type, const String& name) const;

		/**	Gets a descriptor for a parameter block buffer with the specified name. */
		GpuParamBlockDesc* GetParamBlockDesc(GpuProgramType type, const String& name) const;

		/**
		 * @copydoc te::GpuParams::Create(const SPtr<GraphicsPipelineState>&, GpuDeviceFlags)
		 * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
		 */
		static SPtr<GpuParams> Create(const SPtr<GraphicsPipelineState>& pipelineState,
			GpuDeviceFlags deviceMask = GDF_DEFAULT);

		/**
		 * @copydoc te::GpuParams::Create(const SPtr<GpuPipelineParamInfo>&, GpuDeviceFlags)
		 * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
		 */
		static SPtr<GpuParams> Create(const SPtr<GpuPipelineParamInfo>& paramInfo,
			GpuDeviceFlags deviceMask = GDF_DEFAULT);

		// TODO

	protected:
		friend class HardwareBufferManager;

		GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask);

		/**	Gets a descriptor for a data parameter with the specified name. */
		GpuParamDataDesc* GetParamDesc(GpuProgramType type, const String& name) const;

		/** @copydoc CoreObject::GetThisPtr */
		SPtr<GpuParams> _getThisPtr() const;

	protected:
		SPtr<GpuPipelineParamInfo> _paramInfo;

		/** Data for a single bound texture. */
		struct TextureData
		{
			TextureType Texture;
			TextureSurface Surface;
		};

		GpuParamBlockBuffer* _paramBlockBuffers = nullptr;
		TextureData* _sampledTextureData = nullptr;
		GpuBuffer* _buffers = nullptr;
		SamplerState* _samplerStates = nullptr;
	};
}