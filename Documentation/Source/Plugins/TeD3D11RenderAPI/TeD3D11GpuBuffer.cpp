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
        ClearBufferViews();
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
        _bufferView = RequestView(this, 0, props.GetElementCount(), (GpuViewUsage)usage);

        GpuBuffer::Initialize();
    }

    ID3D11Buffer* D3D11GpuBuffer::GetDX11Buffer() const
    {
        return static_cast<D3D11HardwareBuffer*>(_buffer)->GetD3DBuffer();
    }

    GpuBufferView* D3D11GpuBuffer::RequestView(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 numElements,
        GpuViewUsage usage)
    {
        const auto& props = buffer->GetProperties();

        GPU_BUFFER_VIEW_DESC key;
        key.FirstElement = firstElement;
        key.ElementWidth = props.GetElementSize();
        key.NumElements = numElements;
        key.Usage = usage;
        key.Format = props.GetFormat();
        key.UseCounter = false;

        auto iterFind = buffer->_bufferViews.find(key);
        if (iterFind == buffer->_bufferViews.end())
        {
            GpuBufferView* newView = te_new<GpuBufferView>();
            newView->Initialize(buffer, key);
            buffer->_bufferViews[key] = te_new<GpuBufferReference>(newView);

            iterFind = buffer->_bufferViews.find(key);
        }

        iterFind->second->RefCount++;
        return iterFind->second->View;
    }

    void D3D11GpuBuffer::ReleaseView(GpuBufferView* view)
    {
        D3D11GpuBuffer* buffer = view->GetBuffer();

        auto iterFind = buffer->_bufferViews.find(view->GetDesc());
        if (iterFind == buffer->_bufferViews.end())
        {
            TE_ASSERT_ERROR(false, "Trying to release a buffer view that doesn't exist!", __FILE__, __LINE__);
        }

        iterFind->second->RefCount--;

        if (iterFind->second->RefCount == 0)
        {
            GpuBufferReference* toRemove = iterFind->second;

            buffer->_bufferViews.erase(iterFind);

            if (toRemove->View != nullptr)
            {
                te_delete(toRemove->View);
            }

            te_delete(toRemove);
        }
    }

    void D3D11GpuBuffer::ClearBufferViews()
    {
        for (auto iter = _bufferViews.begin(); iter != _bufferViews.end(); ++iter)
        {
            if (iter->second->View != nullptr)
            {
                te_delete(iter->second->View);
            }

            te_delete(iter->second);
        }

        _bufferViews.clear();
    }

    ID3D11ShaderResourceView* D3D11GpuBuffer::GetSRV() const
    {
        return _bufferView->GetSRV();
    }

    ID3D11UnorderedAccessView* D3D11GpuBuffer::GetUAV() const
    {
        return _bufferView->GetUAV();
    }
}
