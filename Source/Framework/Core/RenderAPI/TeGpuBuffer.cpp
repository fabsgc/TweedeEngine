#include "TeGpuBuffer.h"
#include "TeHardwareBufferManager.h"

namespace te
{
    UINT32 GetBufferSize(const GPU_BUFFER_DESC& desc)
    {
        UINT32 elementSize;

        if (desc.Type == GBT_STANDARD)
        {
            elementSize = GpuBuffer::GetFormatSize(desc.Format);
        }
        else
        {
            elementSize = desc.ElementSize;
        }

        return elementSize * desc.ElementCount;
    }

    GpuBufferProperties::GpuBufferProperties(const GPU_BUFFER_DESC& desc)
        : _desc(desc)
    {
        if(_desc.Type == GBT_STANDARD)
        {
            _desc.ElementSize = GpuBuffer::GetFormatSize(_desc.Format);
        }
    }

    GpuBuffer::GpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : HardwareBuffer(GetBufferSize(desc)
        , desc.Usage, deviceMask)
        , _properties(desc)
    {
        if (desc.Type != GBT_STANDARD)
        {
            assert(desc.Format == BF_UNKNOWN && "Format must be set to BF_UNKNOWN when using non-standard buffers");
        }
        else
        {
            assert(desc.ElementSize == 0 && "No element size can be provided for standard buffer. Size is determined from format.");
        }
    }

    GpuBuffer::GpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer)
        : HardwareBuffer(GetBufferSize(desc)
        , desc.Usage
        , underlyingBuffer->GetDeviceMask())
        , _properties(desc)
        , _buffer(underlyingBuffer.get())
        , _sharedBuffer(std::move(underlyingBuffer))
        , _isExternalBuffer(true)
    {
        const auto& props = GetProperties();
        assert(_sharedBuffer->GetSize() == (props.GetElementCount() * props.GetElementSize()));

        if (desc.Type != GBT_STANDARD)
        {
            assert(desc.Format == BF_UNKNOWN && "Format must be set to BF_UNKNOWN when using non-standard buffers");
        }
        else
        {
            assert(desc.ElementSize == 0 && "No element size can be provided for standard buffer. Size is determined from format.");
        }
    }

    GpuBuffer::~GpuBuffer()
    {
        if(_buffer && !_sharedBuffer)
        {
            _bufferDeleter(_buffer);
        }
    }

    void GpuBuffer::Initialize()
    {
        CoreObject::Initialize();
    }

    void* GpuBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        return _buffer->Lock(offset, length, options, deviceIdx, queueIdx);
    }

    void GpuBuffer::Unmap()
    {
        _buffer->Unlock();
    }

    void GpuBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        _buffer->ReadData(offset, length, dest, deviceIdx, queueIdx);
    }

    void GpuBuffer::WriteData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        _buffer->WriteData(offset, length, source, writeFlags, queueIdx);
    }

    void GpuBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length,
        bool discardWholeBuffer)
    {
        auto& srcGpuBuffer = static_cast<GpuBuffer&>(srcBuffer);
        _buffer->CopyData(*srcGpuBuffer._buffer, srcOffset, dstOffset, length, discardWholeBuffer);
    }

    SPtr<GpuBuffer> GpuBuffer::GetView(GpuBufferType type, GpuBufferFormat format, UINT32 elementSize)
    {
        const UINT32 elemSize = type == GBT_STANDARD ? te::GpuBuffer::GetFormatSize(format) : elementSize;
        if ((_buffer->GetSize() % elemSize) != 0)
        {
            TE_DEBUG("Size of the buffer isn't divisible by individual element size provided for the buffer view.", __FILE__, __LINE__);
            return nullptr;
        }

        GPU_BUFFER_DESC desc;
        desc.Type = type;
        desc.Format = format;
        desc.Usage = _usage;
        desc.ElementSize = elementSize;
        desc.ElementCount = _buffer->GetSize() / elemSize;

        if (!_sharedBuffer)
        {
            _sharedBuffer = te_shared_ptr(_buffer, _bufferDeleter);
            _isExternalBuffer = false;
        }

        SPtr<GpuBuffer> newView = Create(desc, _sharedBuffer);
        return newView;
    }

    SPtr<GpuBuffer> GpuBuffer::Create(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuBuffer(desc, deviceMask);
    }

    SPtr<GpuBuffer> GpuBuffer::Create(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer)
    {
        return HardwareBufferManager::Instance().CreateGpuBuffer(desc, std::move(underlyingBuffer));
    }

    UINT32 GpuBuffer::GetFormatSize(GpuBufferFormat format)
    {
        static bool lookupInitialized = false;

        static UINT32 lookup[BF_COUNT];
        if (!lookupInitialized)
        {
            lookup[BF_16X1F] = 2;
            lookup[BF_16X2F] = 4;
            lookup[BF_16X4F] = 8;
            lookup[BF_32X1F] = 4;
            lookup[BF_32X2F] = 8;
            lookup[BF_32X3F] = 12;
            lookup[BF_32X4F] = 16;
            lookup[BF_8X1] = 1;
            lookup[BF_8X2] = 2;
            lookup[BF_8X4] = 4;
            lookup[BF_16X1] = 2;
            lookup[BF_16X2] = 4;
            lookup[BF_16X4] = 8;
            lookup[BF_8X1S] = 1;
            lookup[BF_8X2S] = 2;
            lookup[BF_8X4S] = 4;
            lookup[BF_16X1S] = 2;
            lookup[BF_16X2S] = 4;
            lookup[BF_16X4S] = 8;
            lookup[BF_32X1S] = 4;
            lookup[BF_32X2S] = 8;
            lookup[BF_32X3S] = 12;
            lookup[BF_32X4S] = 16;
            lookup[BF_8X1U] = 1;
            lookup[BF_8X2U] = 2;
            lookup[BF_8X4U] = 4;
            lookup[BF_16X1U] = 2;
            lookup[BF_16X2U] = 4;
            lookup[BF_16X4U] = 8;
            lookup[BF_32X1U] = 4;
            lookup[BF_32X2U] = 8;
            lookup[BF_32X3U] = 12;
            lookup[BF_32X4U] = 16;

            lookupInitialized = true;
        }

        if (format >= BF_COUNT)
            return 0;

        return lookup[(UINT32)format];
    }
}
