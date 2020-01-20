#include "TeD3D11HardwareBufferManager.h"
#include "TeD3D11VertexBuffer.h"
#include "TeD3D11IndexBuffer.h"
#include "TeD3D11GpuParamBlockBuffer.h"

namespace te
{
    D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11Device& device)
        : _device(device)
    { }

    SPtr<VertexBuffer> D3D11HardwareBufferManager::CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<D3D11VertexBuffer> ret = te_shared_ptr_new<D3D11VertexBuffer>(_device, desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<IndexBuffer> D3D11HardwareBufferManager::CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<D3D11IndexBuffer> ret = te_shared_ptr_new<D3D11IndexBuffer>(_device, desc, deviceMask);
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
}
