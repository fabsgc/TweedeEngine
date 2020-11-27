#include "TeD3D11GpuParamBlockBuffer.h"
#include "TeD3D11HardwareBuffer.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"

namespace te
{
    D3D11GpuParamBlockBuffer::D3D11GpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask)
        :GpuParamBlockBuffer(size, usage, deviceMask)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11GpuParamBlockBuffer::~D3D11GpuParamBlockBuffer()
    {
        if (_buffer != nullptr)
            te_pool_delete(static_cast<D3D11HardwareBuffer*>(_buffer));
    }

    void D3D11GpuParamBlockBuffer::Initialize()
    {
        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = d3d11rs->GetPrimaryDevice();

        _buffer = te_pool_new<D3D11HardwareBuffer>(D3D11HardwareBuffer::BT_CONSTANT, _usage, 1, _size, device);
        GpuParamBlockBuffer::Initialize();
    }

    ID3D11Buffer* D3D11GpuParamBlockBuffer::GetD3D11Buffer() const
    {
        return static_cast<D3D11HardwareBuffer*>(_buffer)->GetD3DBuffer();
    }
}
