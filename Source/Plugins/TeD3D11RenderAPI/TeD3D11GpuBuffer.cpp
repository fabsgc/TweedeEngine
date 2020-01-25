#include "TeD3D11GpuBuffer.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11HardwareBuffer.h"
#include "TeD3D11Device.h"
#include "TeD3D11Mappings.h"

namespace te
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_delete(static_cast<D3D11HardwareBuffer*>(buffer));
    }

    D3D11GpuBuffer::D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : GpuBuffer(desc, deviceMask)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11GpuBuffer::D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer)
        : GpuBuffer(desc, std::move(underlyingBuffer))
    { }

    D3D11GpuBuffer::~D3D11GpuBuffer()
    {
        // clearBufferViews(); TODO
    }

    void D3D11GpuBuffer::Initialize()
    {
        const GpuBufferProperties& props = GetProperties();
        _bufferDeleter = &DeleteBuffer;

        // Create a new buffer if not wrapping an external one
        if(!_buffer)
        {
            D3D11HardwareBuffer::BufferType bufferType;
            D3D11RenderAPI* rapi = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());

            switch (props.GetType())
            {
            case GBT_STANDARD:
                bufferType = D3D11HardwareBuffer::BT_STANDARD;
                break;
            case GBT_STRUCTURED:
                bufferType = D3D11HardwareBuffer::BT_STRUCTURED;
                break;
            case GBT_INDIRECTARGUMENT:
                bufferType = D3D11HardwareBuffer::BT_INDIRECTARGUMENT;
                break;
            default:
                TE_ASSERT_ERROR(false, "Unsupported buffer type " + ToString(props.GetType()), __FILE__, __LINE__);
            }

            _buffer = te_new<D3D11HardwareBuffer>(bufferType, props.GetUsage(), props.GetElementCount(),
                props.GetElementSize(), rapi->GetPrimaryDevice(), false, false);
        }

        UINT32 usage = GVU_DEFAULT;
        if ((props.GetUsage() & GBU_LOADSTORE) == GBU_LOADSTORE)
        {
            usage |= GVU_RANDOMWRITE;
        }

        // Keep a single view of the entire buffer, we don't support views of sub-sets (yet)
        // _bufferView = requestView(this, 0, props.GetElementCount(), (GpuViewUsage)usage); TODO

        GpuBuffer::Initialize();
    }
}
