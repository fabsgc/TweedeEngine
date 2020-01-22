#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
	/**	DirectX 11 implementation of a generic GPU buffer. */
	class D3D11GpuBuffer : public GpuBuffer
	{
	public:
		~D3D11GpuBuffer();

	protected:
		friend class D3D11HardwareBufferManager;

		D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
		D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

		/** @copydoc GpuBuffer::Initialize */
		void Initialize() override;

		// TODO
	};
}