#include "TeD3D11VertexBuffer.h"
#include "TeD3D11Device.h"

namespace te 
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_pool_delete(static_cast<D3D11HardwareBuffer*>(buffer));
    }

    D3D11VertexBuffer::D3D11VertexBuffer(D3D11Device& device, const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : VertexBuffer(desc, deviceMask)
        , _device(device)
        , _streamOut(desc.StreamOut)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    void D3D11VertexBuffer::Initialize()
    {
        _buffer = te_pool_new<D3D11HardwareBuffer>(D3D11HardwareBuffer::BT_VERTEX, _usage, 1, _size, _device, false, _streamOut);
        _bufferDeleter = &DeleteBuffer;

        VertexBuffer::Initialize();
    }
}
