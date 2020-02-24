#include "TeD3D11HardwareBufferManager.h"
#include "TeD3D11VertexBuffer.h"
#include "TeD3D11IndexBuffer.h"
#include "TeD3D11GpuParamBlockBuffer.h"
#include "TeD3D11GpuBuffer.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(D3D11HardwareBufferManager)

    D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11Device& device)
        : _device(device)
    { }

    SPtr<VertexBuffer> D3D11HardwareBufferManager::CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<D3D11VertexBuffer> ret = te_core_ptr_new<D3D11VertexBuffer>(_device, desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<IndexBuffer> D3D11HardwareBufferManager::CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<D3D11IndexBuffer> ret = te_core_ptr_new<D3D11IndexBuffer>(_device, desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<GpuParamBlockBuffer> D3D11HardwareBufferManager::CreateGpuParamBlockBufferInternal(UINT32 size,
        GpuBufferUsage usage, GpuDeviceFlags deviceMask)
    {
        D3D11GpuParamBlockBuffer* paramBlockBuffer =
            new (te_allocate<D3D11GpuParamBlockBuffer>()) D3D11GpuParamBlockBuffer(size, usage, deviceMask);

        SPtr<GpuParamBlockBuffer> paramBlockBufferPtr = te_core_ptr<D3D11GpuParamBlockBuffer>(paramBlockBuffer);
        paramBlockBufferPtr->SetThisPtr(paramBlockBufferPtr);

        return paramBlockBufferPtr;
    }

    SPtr<GpuBuffer> D3D11HardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        D3D11GpuBuffer* buffer = new (te_allocate<D3D11GpuBuffer>()) D3D11GpuBuffer(desc, deviceMask);

        SPtr<D3D11GpuBuffer> bufferPtr = te_core_ptr<D3D11GpuBuffer>(buffer);
        bufferPtr->SetThisPtr(bufferPtr);

        return bufferPtr;
    }

    SPtr<GpuBuffer> D3D11HardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
        SPtr<HardwareBuffer> underlyingBuffer)
    {
        D3D11GpuBuffer* buffer = new (te_allocate<D3D11GpuBuffer>()) D3D11GpuBuffer(desc, std::move(underlyingBuffer));

        SPtr<D3D11GpuBuffer> bufferPtr = te_core_ptr<D3D11GpuBuffer>(buffer);
        bufferPtr->SetThisPtr(bufferPtr);

        return bufferPtr;
    }
}
