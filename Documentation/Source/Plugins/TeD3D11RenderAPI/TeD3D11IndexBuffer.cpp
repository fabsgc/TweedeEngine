#include "TeD3D11IndexBuffer.h"
#include "TeD3D11Device.h"

namespace te 
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_delete(static_cast<D3D11HardwareBuffer*>(buffer));
    }

    D3D11IndexBuffer::D3D11IndexBuffer(D3D11Device& device, const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : IndexBuffer(desc, deviceMask)
        , _device(device)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX.");
    }

    void D3D11IndexBuffer::Initialize()
    {
        _buffer = te_new<D3D11HardwareBuffer>(D3D11HardwareBuffer::BT_INDEX, _usage, 1, _size, _device);
        _bufferDeleter = &DeleteBuffer;

        IndexBuffer::Initialize();
    }
}
