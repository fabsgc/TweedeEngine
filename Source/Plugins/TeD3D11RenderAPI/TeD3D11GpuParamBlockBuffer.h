#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuParamBlockBuffer.h"

namespace te
{
	/**	DirectX 11 implementation of a parameter block buffer (constant buffer in DX11 lingo). */
	class D3D11GpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		D3D11GpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask);
		~D3D11GpuParamBlockBuffer();

		/**	Returns internal DX11 buffer object. */
		ID3D11Buffer* GetD3D11Buffer() const;
	protected:
		/** @copydoc GpuParamBlockBuffer::initialize */
		void Initialize() override;
	};
}
