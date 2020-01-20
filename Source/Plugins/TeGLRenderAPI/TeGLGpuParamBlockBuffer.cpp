#include "TeGLGpuParamBlockBuffer.h"
#include "TeGLHardwareBuffer.h"
#include "TeGLRenderAPI.h"

namespace te
{
	GLGpuParamBlockBuffer::GLGpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask)
		:GpuParamBlockBuffer(size, usage, deviceMask)
	{
		assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
	}

	GLGpuParamBlockBuffer::~GLGpuParamBlockBuffer()
	{
		if (_buffer != nullptr)
        {
			te_delete(static_cast<GLHardwareBuffer*>(_buffer));
        }
	}

	void GLGpuParamBlockBuffer::Initialize()
	{
		GpuParamBlockBuffer::Initialize();
	}
}
