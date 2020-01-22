#include "TeGLGpuBuffer.h"
#include "TeGLRenderAPI.h"
#include "TeGLHardwareBuffer.h"

namespace te
{
	static void DeleteBuffer(HardwareBuffer* buffer)
	{
		te_delete(static_cast<GLHardwareBuffer*>(buffer));
	}

	GLGpuBuffer::GLGpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
		: GpuBuffer(desc, deviceMask)
	{
		assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
	}

	GLGpuBuffer::GLGpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer)
		: GpuBuffer(desc, std::move(underlyingBuffer))
	{ }

	GLGpuBuffer::~GLGpuBuffer()
	{
		// clearBufferViews(); TODO
	}

	void GLGpuBuffer::Initialize()
	{
		const GpuBufferProperties& props = GetProperties();
		_bufferDeleter = &DeleteBuffer;

		// Create a new buffer if not wrapping an external one
		if (!_buffer)
		{
			// TODO
		}

		// TODO
	}
}

