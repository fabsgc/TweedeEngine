#include "TeD3D11HardwareBufferManager.h"
#include "TeD3D11VertexBuffer.h"
#include "TeD3D11IndexBuffer.h"

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
}
