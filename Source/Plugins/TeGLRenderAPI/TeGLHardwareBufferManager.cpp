#include "TeGLHardwareBufferManager.h"
#include "TeGLVertexBuffer.h"
#include "TeGLIndexBuffer.h"
#include "TeGLGpuParamBlockBuffer.h"
#include "TeGLGpuBuffer.h"

namespace te
{
	GLHardwareBufferManager::GLHardwareBufferManager()
	{ }

	SPtr<VertexBuffer> GLHardwareBufferManager::CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		SPtr<GLVertexBuffer> ret = te_shared_ptr_new<GLVertexBuffer>(desc, deviceMask);
		ret->SetThisPtr(ret);

		return ret;
	}

	SPtr<IndexBuffer> GLHardwareBufferManager::CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		SPtr<GLIndexBuffer> ret = te_shared_ptr_new<GLIndexBuffer>(desc, deviceMask);
		ret->SetThisPtr(ret);

		return ret;
	}

	SPtr<GpuParamBlockBuffer> GLHardwareBufferManager::CreateGpuParamBlockBufferInternal(UINT32 size,
		GpuBufferUsage usage, GpuDeviceFlags deviceMask)
	{
		GLGpuParamBlockBuffer* paramBlockBuffer =
			new (te_allocate<GLGpuParamBlockBuffer>()) GLGpuParamBlockBuffer(size, usage, deviceMask);

		SPtr<GpuParamBlockBuffer> paramBlockBufferPtr = te_core_ptr<GLGpuParamBlockBuffer>(paramBlockBuffer);
		paramBlockBufferPtr->SetThisPtr(paramBlockBufferPtr);

		return paramBlockBufferPtr;
	}

	SPtr<GpuBuffer> GLHardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		GLGpuBuffer* buffer = new (te_allocate<GLGpuBuffer>()) GLGpuBuffer(desc, deviceMask);

		SPtr<GLGpuBuffer> bufferPtr = te_core_ptr<GLGpuBuffer>(buffer);
		bufferPtr->SetThisPtr(bufferPtr);

		return bufferPtr;
	}

	SPtr<GpuBuffer> GLHardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
		SPtr<HardwareBuffer> underlyingBuffer)
	{
		GLGpuBuffer* buffer = new (te_allocate<GLGpuBuffer>()) GLGpuBuffer(desc, std::move(underlyingBuffer));

		SPtr<GLGpuBuffer> bufferPtr = te_core_ptr<GLGpuBuffer>(buffer);
		bufferPtr->SetThisPtr(bufferPtr);

		return bufferPtr;
	}
}
