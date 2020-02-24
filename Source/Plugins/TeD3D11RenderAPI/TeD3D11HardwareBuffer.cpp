#include "TeD3D11HardwareBuffer.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11Device.h"

namespace te
{
    D3D11HardwareBuffer::D3D11HardwareBuffer(BufferType btype, GpuBufferUsage usage, UINT32 elementCount, UINT32 elementSize,
        D3D11Device& device, bool useSystemMem, bool streamOut)
        : HardwareBuffer(elementCount* elementSize, usage, GDF_DEFAULT)
        , _bufferType(btype)
        , _elementCount(elementCount)
        , _elementSize(elementSize)
        , _usage(usage)
        , _device(device)
    {
        // TODO load store

        if (useSystemMem)
        {
            if (streamOut)
            {
                TE_DEBUG("useSystemMem and streamOut cannot be used together.", __FILE__, __LINE__);
                streamOut = false;
            }
        }

        if (streamOut)
        {
            if (btype == BT_CONSTANT)
            {
                TE_DEBUG("Constant buffers cannot be used with streamOut.", __FILE__, __LINE__);
                streamOut = false;
            }
        }

        _desc.ByteWidth = GetSize();
        _desc.MiscFlags = 0;
        _desc.StructureByteStride = 0;

        if (useSystemMem)
        {
            _desc.Usage = D3D11_USAGE_STAGING;
            _desc.BindFlags = 0;
            _desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        }
        else
        {
            _desc.Usage = D3D11Mappings::GetUsage(usage);
            _desc.CPUAccessFlags = D3D11Mappings::GetAccessFlags(usage);

            switch (btype)
            {
            default:
            case BT_STANDARD:
                _desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                break;
            case BT_STRUCTURED:
                _desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                _desc.StructureByteStride = elementSize;
                _desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                break;
            case BT_RAW:
                _desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                _desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
                break;
            case BT_VERTEX:
                _desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                break;
            case BT_INDEX:
                _desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                break;
            case BT_CONSTANT:
                _desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                break;
            case BT_INDIRECTARGUMENT:
                _desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                _desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
                break;
            }

            if (streamOut)
            {
                _desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
            }
        }

        HRESULT hr = device.GetD3D11Device()->CreateBuffer(&_desc, nullptr, &_D3DBuffer);
        if (FAILED(hr) || _device.HasError())
        {
            String msg = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create D3D11 buffer: " + msg, __FILE__, __LINE__);
        }
    }

    D3D11HardwareBuffer::~D3D11HardwareBuffer()
    {
        SAFE_RELEASE(_D3DBuffer);

        if (_pTempStagingBuffer != nullptr)
        {
            te_delete(_pTempStagingBuffer);
        }
    }

    void* D3D11HardwareBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        if (length > _size)
            TE_ASSERT_ERROR(false, "Provided length " + ToString(length) + " larger than the buffer " + ToString(_size) + ".", __FILE__, __LINE__);

        // Use direct (and faster) Map/Unmap if dynamic write, or a staging read/write
        if ((_desc.Usage == D3D11_USAGE_DYNAMIC && options != GBL_READ_ONLY) || _desc.Usage == D3D11_USAGE_STAGING)
        {
            D3D11_MAP mapType;

            switch (options)
            {
            case GBL_WRITE_ONLY_DISCARD:
                if (_usage & GBU_DYNAMIC)
                {
                    mapType = D3D11_MAP_WRITE_DISCARD;
                }
                else
                {
                    // Map cannot be called with MAP_WRITE_DISCARD access, because the Resource was not created as
                    // D3D11_USAGE_DYNAMIC. D3D11_USAGE_DYNAMIC Resources must use either MAP_WRITE_DISCARD
                    // or MAP_WRITE_NO_OVERWRITE with Map.
                    mapType = D3D11_MAP_WRITE;
                }
                break;
            case GBL_WRITE_ONLY_NO_OVERWRITE:
                if (_bufferType == BT_INDEX || _bufferType == BT_VERTEX)
                    mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
                else
                {
                    const auto& featureOptions = _device.GetFeatureOptions();

                    if (_bufferType == BT_CONSTANT && featureOptions.MapNoOverwriteOnDynamicConstantBuffer)
                        mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
                    else if (featureOptions.MapNoOverwriteOnDynamicBufferSRV)
                        mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
                    else
                        mapType = D3D11_MAP_WRITE;
                }
                break;
            case GBL_WRITE_ONLY:
                mapType = D3D11_MAP_WRITE;
                break;
            case GBL_READ_WRITE:
                if ((_desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) != 0 &&
                    (_desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) != 0)
                {
                    mapType = D3D11_MAP_READ_WRITE;
                }
                else if (_desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE)
                {
                    mapType = D3D11_MAP_WRITE;
                }
                else
                {
                    mapType = D3D11_MAP_READ;
                }
                break;
            default:
            case GBL_READ_ONLY:
                mapType = D3D11_MAP_READ;
                break;
            }

            if (D3D11Mappings::IsMappingRead(mapType) && (_desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) == 0)
            {
                TE_ASSERT_ERROR(false, "Trying to read a buffer, but buffer wasn't created with a read access flag.", __FILE__, __LINE__);
            }

            if (D3D11Mappings::IsMappingWrite(mapType) && (_desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) == 0)
            {
                TE_DEBUG("Trying to write to a buffer, but buffer wasn't created with a write access flag.", __FILE__, __LINE__);
            }

            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            mappedSubResource.pData = nullptr;
            _device.ClearErrors();

            HRESULT hr = _device.GetImmediateContext()->Map(_D3DBuffer, 0, mapType, 0, &mappedSubResource);
            if (FAILED(hr) || _device.HasError())
            {
                String msg = _device.GetErrorDescription();
                TE_ASSERT_ERROR(false, "Error calling Map: " + msg, __FILE__, __LINE__);
            }

            return static_cast<void*>(static_cast<char*>(mappedSubResource.pData) + offset);
        }
        else // Otherwise create a staging buffer to do all read/write operations on. Usually try to avoid this.
        {
            _useTempStagingBuffer = true;
            if (!_pTempStagingBuffer)
            {
                // Create another buffer instance but use system memory
                _pTempStagingBuffer = te_new<D3D11HardwareBuffer>(_bufferType, GBU_STATIC, 1, _size, std::ref(_device), true);
            }

            // Schedule a copy to the staging
            if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
            {
                _pTempStagingBuffer->CopyData(*this, 0, 0, _size, true);
            }

            // Register whether we'll need to upload on unlock
            _stagingUploadNeeded = (options != GBL_READ_ONLY);

            return _pTempStagingBuffer->Lock(offset, length, options);
        }
    }

    void D3D11HardwareBuffer::Unmap()
    {
        if (_useTempStagingBuffer)
        {
            _useTempStagingBuffer = false;
            _pTempStagingBuffer->Unlock();

            if (_stagingUploadNeeded)
            {
                CopyData(*_pTempStagingBuffer, 0, 0, _size, true);
            }

            if (_pTempStagingBuffer != nullptr)
            {
                te_delete(_pTempStagingBuffer);
                _pTempStagingBuffer = nullptr;
            }
        }
        else
        {
            _device.GetImmediateContext()->Unmap(_D3DBuffer, 0);
        }
    }

    void D3D11HardwareBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    {
        // If we're copying same-size buffers in their entirety
        if (srcOffset == 0 && dstOffset == 0 &&
            length == _size && _size == srcBuffer.GetSize())
        {
            _device.GetImmediateContext()->CopyResource(_D3DBuffer,
                static_cast<D3D11HardwareBuffer&>(srcBuffer).GetD3DBuffer());
            if (_device.HasError())
            {
                String errorDescription = _device.GetErrorDescription();
                TE_ASSERT_ERROR(false, "Cannot copy D3D11 resource\nError Description:" + errorDescription, __FILE__, __LINE__);
            }
        }
        else
        {
            // Copy subregion
            D3D11_BOX srcBox;
            srcBox.left = (UINT)srcOffset;
            srcBox.right = (UINT)srcOffset + length;
            srcBox.top = 0;
            srcBox.bottom = 1;
            srcBox.front = 0;
            srcBox.back = 1;

            _device.GetImmediateContext()->CopySubresourceRegion(_D3DBuffer, 0, (UINT)dstOffset, 0, 0,
                static_cast<D3D11HardwareBuffer&>(srcBuffer).GetD3DBuffer(), 0, &srcBox);
            if (_device.HasError())
            {
                String errorDescription = _device.GetErrorDescription();
                TE_ASSERT_ERROR(false, "Cannot copy D3D11 subresource region\nError Description:" + errorDescription, __FILE__, __LINE__);
            }
        }
    }

    void D3D11HardwareBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        // There is no functional interface in D3D, just do via manual lock, copy & unlock
        void* pSrc = this->Lock(offset, length, GBL_READ_ONLY);
        memcpy(dest, pSrc, length);
        this->Unlock();
    }

    void D3D11HardwareBuffer::WriteData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        if (_desc.Usage == D3D11_USAGE_DYNAMIC || _desc.Usage == D3D11_USAGE_STAGING)
        {
            GpuLockOptions lockOption = GBL_WRITE_ONLY;
            if (writeFlags == BWT_DISCARD)
                lockOption = GBL_WRITE_ONLY_DISCARD;
            else if (writeFlags == BTW_NO_OVERWRITE)
                lockOption = GBL_WRITE_ONLY_NO_OVERWRITE;

            void* pDst = this->Lock(offset, length, lockOption);
            memcpy(pDst, pSource, length);
            this->Unlock();
        }
        else if (_desc.Usage == D3D11_USAGE_DEFAULT)
        {
            if (_bufferType == BT_CONSTANT)
            {
                assert(offset == 0);
                _device.GetImmediateContext()->UpdateSubresource(_D3DBuffer, 0, nullptr, pSource, 0, 0);
            }
            else
            {
                D3D11_BOX dstBox;
                dstBox.left = (UINT)offset;
                dstBox.right = (UINT)offset + length;
                dstBox.top = 0;
                dstBox.bottom = 1;
                dstBox.front = 0;
                dstBox.back = 1;

                _device.GetImmediateContext()->UpdateSubresource(_D3DBuffer, 0, &dstBox, pSource, 0, 0);
            }
        }
        else
        {
            TE_DEBUG("Trying to write into a buffer with unsupported usage: " + ToString(_desc.Usage), __FILE__, __LINE__);
        }
    }
}
